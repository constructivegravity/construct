//Script to calculate the area metric case
//(c) Jonas Schneider
//
//22 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<lambda_B_:0:0:2:0:{d n}>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<lambda_BB_:0:0:2:0:{d n}>),{c d}))
//27 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<lambda_B.BBB_:2:0:2:0:{e f d n}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<lambda_B.BBB_:2:0:2:0:{c d f n}:no>),{e f}))
//28 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<lambda_BB.BBB_:2:0:2:0:{e f d n}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<lambda_B.BBB_:2:0:2:0:{c d f n}:no>),{e f}))
//29 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<lambda_BBB.BBB_:2:0:2:0:{d n e f}>),{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_B.B_:2:0:2:0:{f n c d}>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_B.BB_:2:0:2:0:{c d f n}:no>),{e f}),2), Symmetrize(Multiply(InverseGamma({m n}),Gamma({c d}),Epsilon({a m e}),#<lambda_B_:0:0:2:0:{f n}>),{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),Gamma({f c}),#<lambda_B_:0:0:2:0:{d n}>),{e f},{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),Gamma({d e}),#<lambda_B_:0:0:2:0:{f n}>),{c d},{e f}),-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),Gamma({f c}),#<lambda_BB_:0:0:2:0:{d n}>),{e f},{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),Gamma({d e}),#<lambda_BB_:0:0:2:0:{f n}>),{c d},{e f}),-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({f n c}),Gamma({d a}),#<lambda_BB_:0:0:2:0:{m e}>),{c d},{e f}),-1))
//30 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<lambda_BB.BBB_:2:0:2:0:{c d f n}:no>),{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<lambda_BB.BBB_:2:0:2:0:{e f d n}:no>),{c d}))
//31 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<lambda_BBB.BBB_:2:0:2:0:{d n e f}>),{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_B.BB_:2:0:2:0:{f n c d}:no>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_BB.BB_:2:0:2:0:{f n c d}>),{e f}),2), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({f n c}),Gamma({d a}),#<lambda_BB_:0:0:2:0:{m e}>),{e f},{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({c d}),Epsilon({m a e}),#<lambda_BB_:0:0:2:0:{f n}>),{e f}),-1))
//32 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<lambda_B.BBB_:2:0:2:0:{d n e f}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_B.BBB_:2:0:2:0:{f n c d}:no>),{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<lambda_BB.BBB_:2:0:2:0:{d n e f}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_BB.BBB_:2:0:2:0:{f n c d}:no>),{e f}))
//50 
Add(Multiply(Gamma({a b}),#<lambda:0:0:0:0:{}>), Scale(#<lambda_B_:0:0:2:0:{a b}>,2), Scale(#<lambda_BB_:0:0:2:0:{a b}>,-2))
//51 
Add(Multiply(Gamma({a b}),#<lambda_B_:0:0:2:0:{c d}>), Scale(Symmetrize(Multiply(Gamma({a c}),#<lambda_B_:0:0:2:0:{d b}>),{c d}),2), Scale(#<lambda_B.B_:2:0:2:0:{a b c d}>,2), Scale(#<lambda_B.BB_:2:0:2:0:{c d a b}:no>,-2))
//52 
Add(Multiply(Gamma({a b}),#<lambda_BB_:0:0:2:0:{c d}>), Scale(Symmetrize(Multiply(Gamma({b c}),#<lambda_BB_:0:0:2:0:{d a}>),{c d}),-2), Scale(#<lambda_BB.BB_:2:0:2:0:{a b c d}>,-2), Scale(#<lambda_B.BB_:2:0:2:0:{a b c d}:no>,2))
//53 
Add(#<lambda_B.BBB_:2:0:2:0:{a b c d}:no>, Scale(#<lambda_BB.BBB_:2:0:2:0:{a b c d}:no>,-1))
//traceconditions
Multiply(InverseGamma({m n}),#<lambda_B.BBB_:2:0:2:0:{a b m n}:no>)
Multiply(InverseGamma({m n}),#<lambda_BB.BBB_:2:0:2:0:{a b m n}:no>)
Multiply(InverseGamma({p q}),#<lambda_BBB.BBB_:2:0:2:0:{m n p q}>)
Multiply(InverseGamma({m n}),#<lambda_BBB.BBB_:2:0:2:0:{m n p q}>)
