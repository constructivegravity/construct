//Script to calculate the area metric case
//(c) Jonas Schneider
//
//2 
Add(#<lambda_B.Bd_:2:0:2:1:{e f c d a}>, Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_BBB.B_:2:0:2:0:{d n e f}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<xi_B.BBB_:2:0:2:0:{c d f n}:no>),{e f}))
//3 
Add(#<lambda_B.BBd_:2:0:2:1:{e f c d a}:no>, Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_BBB.B_:2:0:2:0:{d n e f}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<xi_BB.BBB_:2:0:2:0:{c d f n}:no>),{e f}))
//5 
Add(#<lambda_BB.BBd_:2:0:2:1:{e f c d a}>, Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<xi_BB.BBB_:2:0:2:0:{c d f n}:no>),{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_BBB.BB_:2:0:2:0:{d n e f}:no>),{c d}))
//8 
Add(#<lambda_Bd.BB_:2:1:2:0:{c d a e f}:no>, Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<xi_B.BBB_:2:0:2:0:{c d f n}:no>),{e f}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<xi_BBB.BB_:2:0:2:0:{d n e f}:no>),{c d}),-1))
//10 
Add(#<lambda_BBB.BBBd_:2:0:2:1:{e f c d a}>, Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_BBB.B_:2:0:2:0:{c d f n}:no>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<xi_B.BBB_:2:0:2:0:{d n e f}:no>),{c d}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_BBB.BB_:2:0:2:0:{c d f n}:no>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<xi_BB.BBB_:2:0:2:0:{d n e f}:no>),{c d}),2))
//23c 
Add(#<xi_B.BBB_:2:0:2:0:{a b c d}:no>, Scale(#<xi_BB.BBB_:2:0:2:0:{a b c d}:no>,-1))
//54a 
Add(#<lambda_B.Bd_:2:0:2:1:{a b e f c}>, Scale(#<lambda_Bd.BB_:2:1:2:0:{e f c a b}:no>,-1))
//54b 
Add(#<lambda_B.BBd_:2:0:2:1:{a b e f c}:no>, Scale(#<lambda_BB.BBd_:2:0:2:1:{a b e f c}>,-1))
//58c 
Add(#<xi_BBB.B_:2:0:2:0:{c d a b}:no>, Scale(#<xi_BBB.BB_:2:0:2:0:{c d a b}:no>,-1))
//59a 
Add(Symmetrize(#<lambda_B.Bd_:2:0:2:1:{e f a b c}>,{b c}), Scale(Symmetrize(#<lambda_B.BBd_:2:0:2:1:{e f a b c}:no>,{b c}),-1))
//59b 
Add(Symmetrize(#<lambda_Bd.BB_:2:1:2:0:{a b c e f}:no>,{b c}), Scale(Symmetrize(#<lambda_BB.BBd_:2:0:2:1:{e f a b c}>,{b c}),-1))
//traceconditions
Multiply(InverseGamma({p q}),#<lambda_BBB.BBBd_:2:0:2:1:{m n p q c}>)
Multiply(InverseGamma({m n}),#<lambda_BBB.BBBd_:2:0:2:1:{m n p q c}>)
Multiply(InverseGamma({m n}),#<xi_B.BBB_:2:0:2:0:{a b m n}:no>)
Multiply(InverseGamma({m n}),#<xi_BB.BBB_:2:0:2:0:{a b m n}:no>)
Multiply(InverseGamma({m n}),#<xi_BBB.B_:2:0:2:0:{m n a b}:no>)
Multiply(InverseGamma({m n}),#<xi_BBB.BB_:2:0:2:0:{m n a b}:no>)
