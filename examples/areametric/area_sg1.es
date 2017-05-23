//Script to calculate the area metric case
//(c) Jonas Schneider
//
//0a 
Add(#<lambda_Bdd_:0:0:2:2:{c d a b}>, Scale(#<theta_B.B_:2:0:2:0:{a b c d}>,4), Scale(#<theta_B.BB_:2:0:2:0:{c d a b}:no>,-4))
//0b 
Add(#<lambda_BBdd_:0:0:2:2:{c d a b}>, Scale(#<theta_B.BB_:2:0:2:0:{a b c d}:no>,4), Scale(#<theta_BB.BB_:2:0:2:0:{a b c d}>,-4))
//11 
Add(Scale(#<lambda_B.Bdd_:2:0:2:2:{e f c d a b}>,2), Scale(#<lambda_Bd.Bd_:2:1:2:1:{c d a e f b}>,-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<xi_BBB.Bd_:2:0:2:1:{d n e f b}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_B.BBBd_:2:0:2:1:{c d f n b}:no>),{e f}), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.B_:2:0:2:0:{f b c d}>),{e f}),2), Scale(Multiply(Gamma({e f}),#<theta_B.B_:2:0:2:0:{a b c d}>),2), Scale(Multiply(Gamma({a b}),#<theta_B.B_:2:0:2:0:{e f c d}>),-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.BB_:2:0:2:0:{c d f b}:no>),{e f}),-2), Scale(Multiply(Gamma({e f}),#<theta_B.BB_:2:0:2:0:{c d a b}:no>),-2))
//12 
Add(Scale(#<lambda_B.BBdd_:2:0:2:2:{e f c d a b}:no>,2), Scale(#<lambda_Bd.BBd_:2:1:2:1:{e f b c d a}:no>,-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<xi_BBB.Bd_:2:0:2:1:{d n e f b}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_BB.BBBd_:2:0:2:1:{c d f n b}:no>),{e f}), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.BB_:2:0:2:0:{f b c d}:no>),{e f}),2), Scale(Multiply(Gamma({e f}),#<theta_B.BB_:2:0:2:0:{a b c d}:no>),2), Scale(Multiply(Gamma({a b}),#<theta_B.BB_:2:0:2:0:{e f c d}:no>),-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_BB.BB_:2:0:2:0:{f b c d}>),{e f}),-2), Scale(Multiply(Gamma({e f}),#<theta_BB.BB_:2:0:2:0:{a b c d}>),-2))
//14 
Add(Scale(#<lambda_Bdd.BB_:2:2:2:0:{c d a b e f}:no>,2), Scale(#<lambda_Bd.BBd_:2:1:2:1:{c d a e f b}:no>,-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<xi_BBB.BBd_:2:0:2:1:{d n e f b}:no>),{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<xi_B.BBBd_:2:0:2:1:{c d f n b}:no>),{e f}),-1), Scale(Multiply(Gamma({e f}),#<theta_B.B_:2:0:2:0:{a b c d}>),2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.B_:2:0:2:0:{f b c d}>),{e f}),-2), Scale(Multiply(Gamma({a b}),#<theta_B.BB_:2:0:2:0:{c d e f}:no>),-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.BB_:2:0:2:0:{c d f b}:no>),{e f}),2), Scale(Multiply(Gamma({e f}),#<theta_B.BB_:2:0:2:0:{c d a b}:no>),-2))
//15 
Add(Scale(#<lambda_BB.BBdd_:2:0:2:2:{e f c d a b}>,2), Scale(#<lambda_BBd.BBd_:2:1:2:1:{c d a e f b}>,-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_BB.BBBd_:2:0:2:1:{c d f n b}:no>),{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<xi_BBB.BBd_:2:0:2:1:{d n e f b}:no>),{c d}), Scale(Multiply(Gamma({e f}),#<theta_B.BB_:2:0:2:0:{a b c d}:no>),2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.BB_:2:0:2:0:{f b c d}:no>),{e f}),-2), Scale(Multiply(Gamma({a b}),#<theta_BB.BB_:2:0:2:0:{e f c d}>),-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_BB.BB_:2:0:2:0:{f b c d}>),{e f}),2), Scale(Multiply(Gamma({e f}),#<theta_BB.BB_:2:0:2:0:{a b c d}>),-2))
//19 
Add(Scale(#<lambda_BBB.BBBdd_:2:0:2:2:{e f c d a b}>,2), Scale(#<lambda_BBBd.BBBd_:2:1:2:1:{c d a e f b}>,-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<xi_BBB.Bd_:2:0:2:1:{c d f n b}:no>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_B.BBBd_:2:0:2:1:{d n e f b}:no>),{c d}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<xi_BBB.BBd_:2:0:2:1:{c d f n b}:no>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_BB.BBBd_:2:0:2:1:{d n e f b}:no>),{c d}),2), Scale(Multiply(Gamma({a b}),#<theta_BBB.BBB_:2:0:2:0:{e f c d}>),-2))
//26 
Add(Scale(Symmetrize(#<xi_B.BBBd_:2:0:2:1:{a b e f c}:no>,{b c}),2), Scale(Symmetrize(#<xi_BB.BBBd_:2:0:2:1:{a b e f c}:no>,{b c}),-2), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_Bdd_:0:0:2:2:{f n b c}>),{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_BBdd_:0:0:2:2:{f n b c}>),{e f}))
//35 
Add(Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<theta_BBB.BBB_:2:0:2:0:{f n c d}>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<theta_B.B_:2:0:2:0:{d n e f}>),{c d}),4), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<theta_B.BB_:2:0:2:0:{e f d n}:no>),{c d}),4), Scale(#<xi_BBB.Bd_:2:0:2:1:{c d e f a}:no>,-1), Scale(#<xi_B.BBBd_:2:0:2:1:{e f c d a}:no>,-1))
//38 
Add(Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<theta_BBB.BBB_:2:0:2:0:{f n c d}>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<theta_B.BB_:2:0:2:0:{d n e f}:no>),{c d}),-4), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<theta_BB.BB_:2:0:2:0:{d n e f}>),{c d}),4), Scale(#<xi_BBB.BBd_:2:0:2:1:{c d e f a}:no>,-1), Scale(#<xi_BB.BBBd_:2:0:2:1:{e f c d a}:no>,-1))
//40 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_Bdd_:0:0:2:2:{f n b c}>),{e f},{a b c}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_BBdd_:0:0:2:2:{f n b c}>),{a b c},{e f}))
//43 
Add(Symmetrize(Multiply(InverseGamma({m n}),Gamma({e f}),Epsilon({a m g}),#<lambda_Bdd_:0:0:2:2:{h n b c}>),{g h},{a b c}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),Gamma({h e}),#<lambda_Bdd_:0:0:2:2:{f n b c}>),{g h},{e f},{a b c}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),Gamma({f g}),#<lambda_Bdd_:0:0:2:2:{h n b c}>),{e f},{g h},{a b c}),-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a g m}),Gamma({h e}),#<lambda_BBdd_:0:0:2:2:{f n b c}>),{a b c},{g h},{e f}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a e m}),Gamma({f g}),#<lambda_BBdd_:0:0:2:2:{h n b c}>),{g h},{e f},{a b c}),-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({a e}),Epsilon({f g m}),#<lambda_BBdd_:0:0:2:2:{h n b c}>),{a b c},{e f},{g h}),-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a e m}),#<lambda_BBB.BBBdd_:2:0:2:2:{g h f n b c}>),{a b c},{e f}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),#<lambda_B.Bdd_:2:0:2:2:{e f h n b c}>),{g h},{a b c}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),#<lambda_B.BBdd_:2:0:2:2:{e f h n b c}:no>),{g h},{a b c}),2))
//45 
Add(Symmetrize(Multiply(InverseGamma({m n}),Gamma({a e}),Epsilon({f g m}),#<lambda_BBdd_:0:0:2:2:{h n b c}>),{a b c},{e f},{g h}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a g m}),Gamma({e f}),#<lambda_BBdd_:0:0:2:2:{h n b c}>),{a b c},{g h}),-1), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a e m}),#<lambda_BBB.BBBdd_:2:0:2:2:{g h f n b c}>),{a b c},{e f}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),#<lambda_Bdd.BB_:2:2:2:0:{h n b c e f}:no>),{a b c},{g h}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),#<lambda_BB.BBdd_:2:0:2:2:{e f h n b c}>),{a b c},{g h}),2))
//55 
Add(Scale(Symmetrize(Multiply(Gamma({b c}),#<lambda_Bdd_:0:0:2:2:{e f d a}>),{c d}),-2), Scale(#<lambda_B.Bdd_:2:0:2:2:{a b e f c d}>,2), Multiply(Gamma({a b}),#<lambda_Bdd_:0:0:2:2:{e f c d}>), Scale(#<lambda_Bdd.BB_:2:2:2:0:{e f c d a b}:no>,-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<lambda_Bdd_:0:0:2:2:{f b c d}>),{e f}),2))
//56 
Add(Scale(Symmetrize(Multiply(Gamma({b c}),#<lambda_BBdd_:0:0:2:2:{e f d a}>),{c d}),-2), Scale(#<lambda_B.BBdd_:2:0:2:2:{a b e f c d}:no>,2), Multiply(Gamma({a b}),#<lambda_BBdd_:0:0:2:2:{e f c d}>), Scale(#<lambda_BB.BBdd_:2:0:2:2:{a b e f c d}>,-2), Scale(Symmetrize(Multiply(Gamma({b e}),#<lambda_BBdd_:0:0:2:2:{f a c d}>),{e f}),-2))
//60 
Add(Multiply(Gamma({a b}),#<lambda_Bdd_:0:0:2:2:{e f c d}>), Scale(Symmetrize(#<lambda_Bd.Bd_:2:1:2:1:{a c d e f b}>,{c d}),-2), Scale(Symmetrize(#<lambda_Bd.BBd_:2:1:2:1:{e f b a c d}:no>,{c d}),2), Scale(Symmetrize(Multiply(Gamma({c e}),#<lambda_Bdd_:0:0:2:2:{f a d b}>),{c d},{e f}),-4))
//61 
Add(Multiply(Gamma({a b}),#<lambda_BBdd_:0:0:2:2:{e f c d}>), Scale(Symmetrize(#<lambda_Bd.BBd_:2:1:2:1:{a c d e f b}:no>,{c d}),-2), Scale(Symmetrize(#<lambda_BBd.BBd_:2:1:2:1:{a c d e f b}>,{c d}),2), Scale(Symmetrize(Multiply(Gamma({a e}),#<lambda_BBdd_:0:0:2:2:{f c d b}>),{e f},{c d}),4))
//64 
Add(Symmetrize(#<lambda_Bdd_:0:0:2:2:{a b c d}>,{b c d}), Scale(Symmetrize(#<lambda_BBdd_:0:0:2:2:{a b c d}>,{b c d}),-1))
//65 
Add(Symmetrize(#<lambda_B.Bdd_:2:0:2:2:{e f a b c d}>,{b c d}), Scale(Symmetrize(#<lambda_B.BBdd_:2:0:2:2:{e f a b c d}:no>,{b c d}),-1), Symmetrize(Multiply(Gamma({b e}),#<lambda_Bdd_:0:0:2:2:{f a c d}>),{b c d},{e f}))
//66 
Add(Symmetrize(#<lambda_Bdd.BB_:2:2:2:0:{a b c d e f}:no>,{b c d}), Scale(Symmetrize(#<lambda_BB.BBdd_:2:0:2:2:{e f a b c d}>,{b c d}),-1), Scale(Symmetrize(Multiply(Gamma({a e}),#<lambda_BBdd_:0:0:2:2:{f b c d}>),{b c d},{e f}),-1))
//69c 
Add(Symmetrize(#<xi_BBB.Bd_:2:0:2:1:{e f a b c}:no>,{b c}), Scale(Symmetrize(#<xi_BBB.BBd_:2:0:2:1:{e f a b c}:no>,{b c}),-1))
//traceconditions
Multiply(InverseGamma({m n}),#<theta_BBB.BBB_:2:0:2:0:{a b m n}>)
Multiply(InverseGamma({m n}),#<theta_BBB.BBB_:2:0:2:0:{m n a b}>)
Multiply(InverseGamma({p q}),#<lambda_BBB.BBBdd_:2:0:2:2:{m n p q c d}>)
Multiply(InverseGamma({m n}),#<lambda_BBB.BBBdd_:2:0:2:2:{m n p q c d}>)
Multiply(InverseGamma({p q}),#<lambda_BBBd.BBBd_:2:1:2:1:{m n a p q c}>)
Multiply(InverseGamma({m n}),#<lambda_BBBd.BBBd_:2:1:2:1:{m n a p q c}>)
Multiply(InverseGamma({m n}),#<xi_B.BBBd_:2:0:2:1:{a b m n p}:no>)
Multiply(InverseGamma({m n}),#<xi_BB.BBBd_:2:0:2:1:{a b m n p}:no>)
Multiply(InverseGamma({m n}),#<xi_BBB.Bd_:2:0:2:1:{m n a b c}:no>)
Multiply(InverseGamma({m n}),#<xi_BBB.BBd_:2:0:2:1:{m n a b c}:no>)
