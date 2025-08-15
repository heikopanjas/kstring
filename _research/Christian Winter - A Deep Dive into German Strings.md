---
from: https://cedardb.com/blog/strings_deep_dive/
---

A Deep Dive into German Strings
cedardb.com

“Strings are Everywhere”! At least according to a 2018 DBTest Paper from the
Hyper team at Tableau. In fact, strings make up nearly half of the data processed
at Tableau. This high prevalence undoubtedly applies to many other companies
as well, as the paper’s dataset consists of data analyzed by Tableau’s users. The
string-heavy nature of the data makes string processing one of the most
important tasks of a database system.
In our previous blog post, we introduced you to our optimized string layout
German Strings, which was already present in the Hyper system and has since
been widely adopted. Our post received a lot of attention and, more importantly,
questions about their inner workings. We want to take this oportunity to dive
deeper into the nitty-gritty details of their implementation in this post and expand
on why the optimizations described in our previous post are necessary for highly
performant string processing.

Beneﬁts of a 16B String Representation
First of all, we would like to explain why it is so important to stick to 16 bytes for
our string representation.

Space Savings
Let’s start with the obvious beneﬁt of using 16 bytes instead of 24 bytes like
C++’s std::string to represent a string: It saves 8 bytes per string. At ﬁrst
glance, these ﬁxed-size savings may seem insigniﬁcant for a datatype of variable
size. After all, it corresponds to only, depending on string encoding, roughly 8
characters when a short blog post like this has thousands. However, the database
system only needs to load the string when it needs to interact with it. For all
operators that do not directly modify, aggregate, or sort on the string, the string
representation is all that matters.
For example, consider TPC-H query 21 and the output column s_name .
Depending on the query plan, 66% of operators processing this supplier name
s_name will materialize its string representation without accessing the underlying

string values at all. For all of these operators, storing only 16 bytes instead of 24
directly reduces the space required for this column by 33%.

Function Call Optimizations
A way less obivous, but not insigniﬁcant, beneﬁt we get from smaller string
headers is that it is vastly more eﬃcient to pass our strings to and from functions.
The System V calling convention mandates that large structs need to be passed
through the stack. However, values up to 16 B will be passed through two
registers.
Staying within the magic limit of 16 B is therefore very beneﬁcial, as it avoids a
costly round-trip through the stack at every function call. Consider the following
small example where we pass two strings to a compare() function:
#include <cstdint>
#include <string>
struct data128_t { uint64_t v[2]; };
void compare(data128_t, data128_t);
void compare(std::string, std::string);
void compareData128() {
data128_t abc = {0x0300'0000'4142'4300, 0x0300'0000'0000'0000};
data128_t def = {0x0300'0000'4445'4600, 0x0300'0000'0000'0000};
compare(abc, def);
}
void compareStrings() {
std::string abc = "abc";
std::string def = "def";
compare(move(abc), move(def));
}

Both compareData128 and compareStrings make a call to an unspeciﬁed
compare function and both compare the strings "abc" and "def" . However,
compareData128 uses our 16-byte representation while compareStrings uses the
std::string representation. If you take a quick look at the generated assembly in

the Compiler Explorer, you can see a dramatic diﬀerence in the generated code
for calls to both functions. Our string format, simulated here with a small struct,
is passed directly via CPU registers, resulting in only 4 instructions to execute.
The std::string version of the same function, on the other hand, requires a
whopping 37 instructions to set up the call. While some of this is due to the mess
of C++’s non-destructive moves, there is still a signiﬁcant diﬀerence in the setup
of the function call. And with the amount of data that a database system
processes on a daily basis, such small diﬀerences add up quickly!

Detour: Pointer Tagging
Many of the questions we received were about our use of pointer tagging, which
we need to do to stay within the 16-byte limit and get the beneﬁts outlined above.
While pointer tagging is in no way speciﬁc to German Strings, we want to take a
little detour to answer the most frequently asked questions.
Why are you using the most signiﬁcant bits for tagging?
Pointers can be tagged either in the most signiﬁcant bits, i.e. the bits that are not
yet used, since there are no machines with 128 PiB of RAM, or in the least
signiﬁcant bits. Using tags in the least signiﬁcant bits has the advantage that these
bits are not aﬀected by architecture changes, they are already in use today. And
that last part is the problem with using them for tagging. To be able to tag the
least-signiﬁcant two bits, all data must be aligned to 4 bytes, so that these two bits
are always guaranteed to be zero. While some datatypes are word-aligned, we
want to store our strings as compactly as possible, and therefore require full byteaddressability of our pointers. This rules out the least signiﬁcant bits for tagging.
Is pointer tagging safe?
It depends. While dereferencing a tagged pointer as-is is not possible on some
architectures, no architecture uses all 64 bits of an address. In fact, even the latest
extension to x86-64 uses at most 57 bits, leaving 7 bits for the application to
cram more information into a pointer. However, some architectures, including
x86-64, require pointers to be in canonical form for all memory operations.
Therefore, we must make sure to remove all tags before dereferencing the tagged
pointer. So for the foreseeable future, it is safe to use pointers tagged in the most
signiﬁcant bits, as long as we remove the tags before using them as actual
pointers to memory. Both least-signiﬁcant and most-signiﬁcant bit tagging
require that the tags be removed before performing memory operations.

Beneﬁts for Handling Short Strings
Let’s get back to our German Strings. Another beneﬁt of our string representation
is that we can work with short strings, i.e. those not longer than 12 characters,
directly in CPU registers. But even that requires additional care. For example,
let’s take a look at string equality checks:
#include <cstdint>
#include <cstring>
struct data128_t { uint64_t v[2]; };
bool isEqual(data128_t a, data128_t b) {
if (a.v[0] != b.v[0]) return false;
auto len = (uint32_t) a.v[0];
if (len <= 12) return a.v[1] == b.v[1];
return memcmp((char*) a.v[1], (char*) b.v[1], len) == 0;
}
bool isEqualAlt(data128_t a, data128_t b) {
auto aLen = (uint32_t) a.v[0];
auto aPtr = aLen <= 12 ? (char*)&a.v + 4 : (char*)a.v[1];
auto bLen = (uint32_t) b.v[0];
auto bPtr = bLen <= 12 ? (char*)&b.v + 4 : (char*)b.v[1];
return memcmp(aPtr, bPtr, aLen) == 0;
}

Both isEqual and isEqualAlt do the same thing. However, in isEqual we add a
special path to take advantage of our string layout. For joins and ﬁlters in a
database system, the vast majority of comparisons will usually result in an
inequality. To optimize for this path, we ﬁrst check that both strings have the
same length and preﬁx, which can be done with a simple register comparison of
the ﬁrst 8 bytes of our string layout. This check will fail for most cases, allowing us
to answer the vast majority of string comparisons without actually looking at the
strings with a very cheap CMP instruction.
For strings that ﬁt into our short string representation, we can add an additional
optimization path. Instead of pushing string pointers to the stack and calling
memcmp , we can answer the equality comparison by simply comparing the second

8 bytes in another integer comparison.
In the generated code on Compiler Explorer, you can see that for the same job,
isEqualAlt has to perform several operations on the stack. From the operations

alone, you can see that this is obviously much more expensive than shuﬄing
registers.

Conclusion
We hope this little deep dive could answer some of the open questions around our
German Strings. If you don’t want to miss out on future deep dives, make sure to
sign up to our waitlist and the attached newsletter!
Join our waitlist!
Join our Waitlist
cedardb.com

