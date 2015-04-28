Output from "growth 20 x 0" where x is in range [4..15]

growth-*-* are raw output.
T is merged and processed output, as follows.

Lines have been tagged with Posix shell script in file T:

for F in growth-*-*; do grep -h . $F | (
  F1=${F#growth-}; X=${F1%-*}; N=${F#growth-??-};
  for S in V- H- VV VH HV HH; do for P in I S; do for B in C R;
  do for M in ND PD PL MD ML XD XL; do read i; echo "$X$N$S$P$B$M, $i";
  done; done; done; done; ) done 

Lines of T are comma-separated values:

TAG, TIME, RELATIVE, STRUCTURE, ALLOCATION, BINDING

TAG is 4 digits and 6 alpha characters, as follows

  2009VHSRML
   | | ||| |
   | | ||| +- Allocate from:
   | | |||  AS1,2   ND: new/delete
   | | |||  AS7,9   PD: multipool, destroy data normally
   | | |||  AS8,10  PL: multipool/drop, leak data to allocator, destroy allocator
   | | |||  AS3,5   MD: monotonic, destroy data normally
   | | |||  AS7,9   ML: monotonic/drop, leak data to allocator, destroy allocator
   | | |||  AS11,13 XD: multipool/monotonic - multipool allocator obtains storage
   | | |||             from monotonic allocator, all destroyed in order
   | | |||  AS12,14 XL: multipool/monotonic - multipool allocator allocates 
   | | |||             from monotonic allocator, data and multipool leaked
   | | |||             to monotonic allocator
   | | ||+- Binding:
   | | ||  AS(odd)  C: compile-time, inline
   | | ||  AS(even) R: run-time, virtual dispatch
   | | ||
   | | |+-- Data item:
   | | |   DS(odd)  I: int
   | | |   DS(even) S: string (33-1000 chars)
   | | |
   | | +--- Data structure:
   | |     DS1,2    V-: vector<T>
   | |     DS3,4    H-: unordered_set<T>
   | |     DS5,6    VV: vector<vector<T>>
   | |     DS7,8    VH: vector<unordered_set<T>> 
   | |     DS9,10   HV: unordered_set<vector<T>> 
   | |     DS11,12  HH: unordered_set<unordered_set<T>>
   | |        
   | +----- M, log2 of number of insertions (9 -> 2^9 = 512) per iteration
   |       For VV,VH,HV,HH, inner containers get 128 elements (int or string),
   |         outer container gets 2^M inner containers
   |       For V-,H-, containers get 2^M elements
   |
   +------- N, log2 of number of iterations (20 -> 1M).
           For VV,VH,HV,HH, number of iterations is 2^N
           For V-,H-, number of iterations is 128 * 2^N

TIME is wall-clock run time in seconds
RELATIVE is ratio of TIME to TIME of compile-time-bound new/delete run
STRUCTURE is a data structure as noted for the tag 3rd through 5th bytes
ALLOCATION is an allocation mode corresponding to the final two tag bytes
BINDING is the calling method for allocation and deallocation, tag 6th byte

T is re-ordered and split into files T-XXX for tables, one file for each
data structure:

$ for i in V- H- VV VH HV HH; do for j in I S; do grep "^....$i$j" T >T-$i$j; done; done

Files T-??? are transformed into comma-separated table files T-???-table.csv:

$ for i in T-???; do
    cat $i | (
      while read tag rest; do
         sz1=${tag#20}; sz=${sz1%%[A-Z]*} ;
         case $tag in
           *CND*) ix=01;; *RND*) ix=02;; *CMD*) ix=03;; *CML*) ix=04;; *RMD*) ix=05;;
           *RML*) ix=06;; *CPD*) ix=07;; *CPL*) ix=08;; *RPD*) ix=09;; *RPL*) ix=10;;
           *CXD*) ix=11;; *CXL*) ix=12;; *RXD*) ix=13;; *RXL*) ix=14;;
         esac; echo $sz$ix $tag $rest; done;
    ) | sort -sn | (
       while read ix tag time rel rest; do
          # (echo "**ix='$ix' tag='$tag' time='$time' rel='$rel'**"; echo) 
          case "$tag" in
              *CND*) echo -n "[${time%,}s], ";;
              *) r=${rel%%%,}; echo -n "${r%.*}, ";;
          esac;
          case "$tag" in *RXL*) echo;; esac; done
    ) |
       sed 's/[(]failed%[)],,/N\/A,/g'   >$i-ASxSZ.csv; done
