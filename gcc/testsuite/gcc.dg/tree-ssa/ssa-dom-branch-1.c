/* { dg-do compile } */ 
/* { dg-options "-O2 -w -fdump-tree-dom1-details" } */

typedef struct rtx_def *rtx;
struct rtx_def
{
  int code;
  rtx rt_rtx;
};
rtx
try_combine (rtx i1, rtx newpat)
{
  rtx temp;
  if (i1 && (temp = ((((((newpat->rt_rtx, ((((temp)->code) == 42)))))))))
      && ((temp =
	(((((((((((newpat)->rt_rtx),
		 ((((temp)->code) == 42) && arf ())))))))))))))
    ;
  else if (i1 && foo ());
}

/* There should be three tests against i1.  Two from the hash table
   dumps, one in the code itself.  */
/* { dg-final { scan-tree-dump-times "if .i1_" 3 "dom1"} } */

/* There should be no actual jump threads realized by DOM.  The
   legitimize jump threads are handled in VRP and those discovered
   by DOM are subsumed by collapsing a conditional.  */
/* { dg-final { scan-tree-dump-not "Threaded" "dom1"} } */
