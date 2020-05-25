# Optimize_Hash_Table
Using asm to speed up hash table 

Hash Table - gets hashes of 290550 english words
Let's see how fast it is

![alt text](profiling.jpg)
# time = 111 seconds 
that is definitely slow. 
However it is clear where is the bottleneck - it's sscanf!
Of course we don't need all sscanf functionality so lets write mini-sscanf on asm that gets word from text.
![alt text](getword.jpg)
let's check how fast is hash table now.
![alt text](profiling_with_gw.jpg)
# time = 2.16 seconds
Wow! That's much better

