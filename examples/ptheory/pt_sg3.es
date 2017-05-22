//Script to calculate the ptheory case
//(c) Jonas Schneider
//
//4
Add(#<lambda_BnCa_:2:0:3:1:{a b c d e f}>, Scale(#<xi_DBn_:4:0:2:0:{c d e f a b}>,-4))
//6
#<lambda_BaCn_:2:1:3:0:{d e f a b c}>
//8
Add(#<lambda_CnDa_:3:0:4:1:{a b c d e f g h}>, Scale(Symmetrize(Multiply(Gamma({h a}),#<xi_DBn_:4:0:2:0:{d e f g b c}>),{a b c}),-2))
//13
Add(#<lambda_CaDn_:3:1:4:0:{e f g h a b c d}>, Scale(Symmetrize(Multiply(Gamma({h e}),#<xi_BDn_:2:0:4:0:{f g a b c d}>),{e f g}),-2))
//25a
Multiply(InverseGamma({m n}),#<xi_DBn_:4:0:2:0:{a b m n p q}>)
//25b
#<xi_BDn_:2:0:4:0:{a b p q r s}>
//43b
Add(#<lambda_BnCa_:2:0:3:1:{a b p q r c}>, Scale(Multiply(InverseGamma({m n}),#<lambda_CaDn_:3:1:4:0:{p q r c a b m n}>),-6))
//45a
Multiply(InverseGamma({m n}),#<xi_BDn_:2:0:4:0:{p q a b m n}>)
//45b
#<xi_DBn_:4:0:2:0:{p q r s a b}>
//46b
Add(Symmetrize(#<lambda_BaCn_:2:1:3:0:{a b c p q r}>,{b c}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_CnDa_:3:0:4:1:{p q r m n a b c}>),{b c}),-6))
