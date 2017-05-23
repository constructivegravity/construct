//Script to calculate the area metric case
//(c) Jonas Schneider
//
//4 
Add(#<lambda_B.BBBd_:2:0:2:1:{e f c d a}:no>, Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_BB.B_:2:0:2:0:{d n e f}:no>),{c d}),-2))
//6 
Add(#<lambda_BB.BBBd_:2:0:2:1:{e f c d a}:no>, Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_B.BB_:2:0:2:0:{d n e f}:no>),{c d}),-2))
//7
Add(#<lambda_Bd.BBB_:2:1:2:0:{c d a e f}:no>, Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_B.BB_:2:0:2:0:{c d f n}:no>),{e f}),2))
//9 
Add(#<lambda_BBd.BBB_:2:1:2:0:{c d a e f}:no>, Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_BB.B_:2:0:2:0:{c d f n}:no>),{e f}),2))
//23a 
#<xi_BB.B_:2:0:2:0:{a b c d}:no>
//23b 
#<xi_B.BB_:2:0:2:0:{a b c d}:no>
//54c 
Add(#<lambda_B.BBBd_:2:0:2:1:{a b e f c}:no>, Scale(#<lambda_BB.BBBd_:2:0:2:1:{a b e f c}:no>,-1))
//58a 
#<xi_B.BB_:2:0:2:0:{c d a b}:no>
//58b 
#<xi_BB.B_:2:0:2:0:{c d a b}:no>
//59c 
Add(Symmetrize(#<lambda_Bd.BBB_:2:1:2:0:{a b c e f}:no>,{b c}), Scale(Symmetrize(#<lambda_BBd.BBB_:2:1:2:0:{a b c e f}:no>,{b c}),-1))
//traceconditions
Multiply(InverseGamma({m n}),#<lambda_B.BBBd_:2:0:2:1:{a b m n c}:no>)
Multiply(InverseGamma({m n}),#<lambda_Bd.BBB_:2:1:2:0:{a b c m n}:no>)
Multiply(InverseGamma({m n}),#<lambda_BB.BBBd_:2:0:2:1:{a b m n e}:no>)
Multiply(InverseGamma({m n}),#<lambda_BBd.BBB_:2:1:2:0:{a b c m n}:no>)
