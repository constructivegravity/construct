//Script to calculate the area metric case
//(c) Jonas Schneider
//
//0c 
Add(#<lambda_BBBdd_:0:0:2:2:{c d a b}>, Scale(#<theta_B.BBB_:2:0:2:0:{a b c d}:no>,4), Scale(#<theta_BB.BBB_:2:0:2:0:{a b c d}:no>,-4))
//13 
Add(Scale(#<lambda_B.BBBdd_:2:0:2:2:{e f c d a b}:no>,2), Scale(#<lambda_Bd.BBBd_:2:1:2:1:{e f b c d a}:no>,-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_BB.Bd_:2:0:2:1:{d n e f b}:no>),{c d}),2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.BBB_:2:0:2:0:{f b c d}:no>),{e f}),2), Scale(Multiply(Gamma({e f}),#<theta_B.BBB_:2:0:2:0:{a b c d}:no>),2), Scale(Multiply(Gamma({a b}),#<theta_B.BBB_:2:0:2:0:{e f c d}:no>),-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_BB.BBB_:2:0:2:0:{f b c d}:no>),{e f}),-2), Scale(Multiply(Gamma({e f}),#<theta_BB.BBB_:2:0:2:0:{a b c d}:no>),-2))
//16 
Add(Scale(#<lambda_BB.BBBdd_:2:0:2:2:{e f c d a b}:no>,2), Scale(#<lambda_BBd.BBBd_:2:1:2:1:{e f b c d a}:no>,-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<xi_B.BBd_:2:0:2:1:{d n e f b}:no>),{c d}),2), Scale(Multiply(Gamma({e f}),#<theta_B.BBB_:2:0:2:0:{a b c d}:no>),2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_B.BBB_:2:0:2:0:{f b c d}:no>),{e f}),-2), Scale(Multiply(Gamma({a b}),#<theta_BB.BBB_:2:0:2:0:{e f c d}:no>),-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<theta_BB.BBB_:2:0:2:0:{f b c d}:no>),{e f}),2), Scale(Multiply(Gamma({e f}),#<theta_BB.BBB_:2:0:2:0:{a b c d}:no>),-2))
//17 
Add(Scale(#<lambda_Bdd.BBB_:2:2:2:0:{c d a b e f}:no>,2), Scale(#<lambda_Bd.BBBd_:2:1:2:1:{c d a e f b}:no>,-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_B.BBd_:2:0:2:1:{c d f n b}:no>),{e f}),-2), Scale(Multiply(Gamma({a b}),#<theta_B.BBB_:2:0:2:0:{c d e f}:no>),-2))
//18 
Add(Scale(#<lambda_BBdd.BBB_:2:2:2:0:{c d a b e f}:no>,2), Scale(#<lambda_BBd.BBBd_:2:1:2:1:{c d a e f b}:no>,-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<xi_BB.Bd_:2:0:2:1:{c d f n b}:no>),{e f}),-2), Scale(Multiply(Gamma({a b}),#<theta_BB.BBB_:2:0:2:0:{c d e f}:no>),-2))
//24 
Add(Scale(Symmetrize(#<xi_BB.Bd_:2:0:2:1:{a b e f c}:no>,{b c}),4), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<lambda_BBBdd_:0:0:2:2:{f n b c}>),{e f}),-1))
//25 
Add(Scale(#<xi_B.BBd_:2:0:2:1:{a b e f c}:no>,4), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<lambda_BBBdd_:0:0:2:2:{f n b c}>),{e f}))
//33 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<theta_B.BBB_:2:0:2:0:{e f d n}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<theta_B.BBB_:2:0:2:0:{c d f n}:no>),{e f}))
//34 
Add(Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<theta_BB.BBB_:2:0:2:0:{c d f n}:no>),{e f}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<theta_B.BBB_:2:0:2:0:{e f d n}:no>),{c d}),2), Scale(#<xi_BB.Bd_:2:0:2:1:{c d e f a}:no>,-1), Scale(#<xi_B.BBd_:2:0:2:1:{e f c d a}:no>,-1))
//36 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<theta_B.BBB_:2:0:2:0:{d n e f}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<theta_B.BBB_:2:0:2:0:{f n c d}:no>),{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a c}),#<theta_BB.BBB_:2:0:2:0:{d n e f}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a e}),#<theta_BB.BBB_:2:0:2:0:{f n c d}:no>),{e f}))
//37 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m c}),#<theta_BB.BBB_:2:0:2:0:{e f d n}:no>),{c d}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<theta_BB.BBB_:2:0:2:0:{c d f n}:no>),{e f}))
//39 
Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a e m}),#<lambda_BBBdd_:0:0:2:2:{f n b c}>),{e f},{a b c})
//41 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),Gamma({f g}),#<lambda_BBBdd_:0:0:2:2:{h n b c}>),{g h},{e f},{a b c}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),Gamma({h e}),#<lambda_BBBdd_:0:0:2:2:{f n b c}>),{e f},{g h},{a b c}), Symmetrize(Multiply(InverseGamma({m n}),Gamma({e f}),Epsilon({a g m}),#<lambda_BBBdd_:0:0:2:2:{h n b c}>),{g h},{a b c}), Symmetrize(Multiply(InverseGamma({m n}),Gamma({g h}),Epsilon({a e m}),#<lambda_BBBdd_:0:0:2:2:{f n b c}>),{e f},{a b c}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a e m}),#<lambda_B.BBBdd_:2:0:2:2:{g h f n b c}:no>),{e f},{a b c}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a g m}),#<lambda_B.BBBdd_:2:0:2:2:{e f h n b c}:no>),{g h},{a b c}),2))
//42 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a g m}),Gamma({h e}),#<lambda_BBBdd_:0:0:2:2:{f n b c}>),{g h},{e f},{a b c}), Symmetrize(Multiply(Epsilon({a e g}),#<lambda_BBBdd_:0:0:2:2:{h f b c}>),{a b c},{e f},{g h}), Symmetrize(Multiply(InverseGamma({m n}),Gamma({e f}),Epsilon({m a g}),#<lambda_BBBdd_:0:0:2:2:{h n b c}>),{a b c},{g h}), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a e m}),#<lambda_BB.BBBdd_:2:0:2:2:{g h f n b c}:no>),{e f},{a b c}),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({m a g}),#<lambda_B.BBBdd_:2:0:2:2:{e f h n b c}:no>),{a b c},{g h}),2))
//44 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a e m}),#<lambda_BB.BBBdd_:2:0:2:2:{g h f n b c}:no>),{a b c},{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a g m}),#<lambda_BB.BBBdd_:2:0:2:2:{e f h n b c}:no>),{a b c},{g h}))
//46 
Add(Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_Bdd.BBB_:2:2:2:0:{f n b c g h}:no>),{a b c},{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),#<lambda_Bdd.BBB_:2:2:2:0:{h n b c e f}:no>),{a b c},{g h}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m e}),#<lambda_BBdd.BBB_:2:2:2:0:{f n b c g h}:no>),{a b c},{e f}), Symmetrize(Multiply(InverseGamma({m n}),Epsilon({a m g}),#<lambda_BBdd.BBB_:2:2:2:0:{h n b c e f}:no>),{a b c},{g h}))
//57 
Add(Scale(Symmetrize(Multiply(Gamma({b c}),#<lambda_BBBdd_:0:0:2:2:{e f d a}>),{c d}),-2), Scale(#<lambda_B.BBBdd_:2:0:2:2:{a b e f c d}:no>,2), Multiply(Gamma({a b}),#<lambda_BBBdd_:0:0:2:2:{e f c d}>), Scale(#<lambda_BB.BBBdd_:2:0:2:2:{a b e f c d}:no>,-2), Symmetrize(Multiply(Gamma({a e}),#<lambda_BBBdd_:0:0:2:2:{f b c d}>),{e f}), Scale(Symmetrize(Multiply(Gamma({b e}),#<lambda_BBBdd_:0:0:2:2:{f a c d}>),{e f}),-1))
//62 
Add(Multiply(Gamma({a b}),#<lambda_BBBdd_:0:0:2:2:{e f c d}>), Scale(Symmetrize(#<lambda_Bd.BBBd_:2:1:2:1:{a c d e f b}:no>,{c d}),-2), Scale(Symmetrize(#<lambda_BBd.BBBd_:2:1:2:1:{a c d e f b}:no>,{c d}),2), Scale(Symmetrize(Multiply(Gamma({c e}),#<lambda_BBBdd_:0:0:2:2:{f a d b}>),{c d},{e f}),-2), Scale(Symmetrize(Multiply(Gamma({a e}),#<lambda_BBBdd_:0:0:2:2:{f c d b}>),{e f},{c d}),2))
//67 
Add(Scale(Symmetrize(#<lambda_Bdd.BBB_:2:2:2:0:{a b c d e f}:no>,{b c d}),2), Scale(Symmetrize(#<lambda_BBdd.BBB_:2:2:2:0:{a b c d e f}:no>,{b c d}),-2), Symmetrize(Multiply(Gamma({b e}),#<lambda_BBBdd_:0:0:2:2:{f a c d}>),{e f},{b c d}), Scale(Symmetrize(Multiply(Gamma({a e}),#<lambda_BBBdd_:0:0:2:2:{f b c d}>),{e f},{b c d}),-1))
//69a 
Symmetrize(#<xi_B.BBd_:2:0:2:1:{e f a b c}:no>,{b c})
//69b 
Symmetrize(#<xi_BB.Bd_:2:0:2:1:{e f a b c}:no>,{b c})
//traceconditions
Multiply(InverseGamma({m n}),#<theta_B.BBB_:2:0:2:0:{a b m n}:no>)
Multiply(InverseGamma({m n}),#<theta_BB.BBB_:2:0:2:0:{a b m n}:no>)
Multiply(InverseGamma({m n}),#<lambda_BBBdd_:0:0:2:2:{m n a b}>)
Multiply(InverseGamma({m n}),#<lambda_B.BBBdd_:2:0:2:2:{a b m n c d}:no>)
Multiply(InverseGamma({m n}),#<lambda_Bd.BBBd_:2:1:2:1:{a b c m n p}:no>)
Multiply(InverseGamma({m n}),#<lambda_Bdd.BBB_:2:2:2:0:{a b c d m n}:no>)
Multiply(InverseGamma({m n}),#<lambda_BB.BBBdd_:2:0:2:2:{a b m n e f}:no>)
Multiply(InverseGamma({m n}),#<lambda_BBd.BBBd_:2:1:2:1:{a b c m n d}:no>)
Multiply(InverseGamma({m n}),#<lambda_BBdd.BBB_:2:2:2:0:{a b c d m n}:no>)
