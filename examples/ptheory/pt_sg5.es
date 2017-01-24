//Script to calculate the ptheory case
//(c) Jonas Schneider
//
//11
Add(Scale(#<lambda_BaCa_:2:1:3:1:{a b g c d e f}>,0.25), Scale(#<lambda_BnCb_:2:0:3:2:{a b c d e f g}>,-0.5), Scale(#<xi_DBa_:4:0:2:1:{c d e f a b g}>,-1))
//15
Add(#<lambda_BaCa_:2:1:3:1:{d e f a b c g}>, Scale(#<lambda_BbCn_:2:2:3:0:{d e f g a b c}>,-2), Scale(#<xi_BDa_:2:0:4:1:{d e a b c f g}>,-4))
//17
Add(#<lambda_CaDa_:3:1:4:1:{a b c i d e f g h}>, Scale(#<lambda_CnDb_:3:0:4:2:{a b c d e f g h i}>,-2), Scale(Symmetrize(Multiply(Gamma({h a}),#<xi_DBa_:4:0:2:1:{d e f g b c i}>),{a b c}),-2))
//19
Add(#<lambda_CaDa_:3:1:4:1:{e f g h a b c d i}>, Scale(#<lambda_CbDn_:3:2:4:0:{e f g h i a b c d}>,-2), Scale(Symmetrize(Multiply(Gamma({h e}),#<xi_BDa_:2:0:4:1:{f g a b c d i}>),{e f g}),-2))
//26
Symmetrize(Multiply(InverseGamma({m n}),#<xi_DBa_:4:0:2:1:{m n c d a b e}>),{d e})
//28
Symmetrize(#<xi_BDa_:2:0:4:1:{e f a b c d g}>,{f g})
//37
Add(#<xi_BDa_:2:0:4:1:{a b c d e f g}>, #<xi_DBa_:4:0:2:1:{c d e f a b g}>)
//44b
Add(Scale(#<lambda_BnCb_:2:0:3:2:{a b p q r c d}>,-1), Scale(Multiply(InverseGamma({m n}),#<lambda_CbDn_:3:2:4:0:{p q r c d m n a b}>),6))
//47b
Add(Symmetrize(#<lambda_BaCa_:2:1:3:1:{a c d p q r b}>,{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_CaDa_:3:1:4:1:{p q r b m n a c d}>),{c d}),-6))
//50b
Add(Symmetrize(#<lambda_BbCn_:2:2:3:0:{a b c d p q r}>,{b c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_CnDb_:3:0:4:2:{p q r m n a b c d}>),{b c d}),-6))
//53a
Symmetrize(Multiply(InverseGamma({m n}),#<xi_BDa_:2:0:4:1:{p q m n a b c}>),{b c})
//53c
Symmetrize(#<xi_DBa_:4:0:2:1:{p q r s a b c}>,{b c})
//57
Add(Symmetrize(Multiply(InverseGamma({m n}),Gamma({e a}),#<lambda_BnCb_:2:0:3:2:{c d b m n f g}>),{a b},{e f g}), Symmetrize(Multiply(InverseGamma({m n}),Gamma({e c}),#<lambda_BnCb_:2:0:3:2:{a b d m n f g}>),{e f g},{c d}), Symmetrize(#<lambda_BnCb_:2:0:3:2:{c d a b e f g}>,{e f g}), Symmetrize(#<lambda_BnCb_:2:0:3:2:{a b c d e f g}>,{e f g}))
//59
Add(Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({g a}),#<lambda_CbDn_:3:2:4:0:{b m n h i c d e f}>),{a b},{g h i}),3), Scale(Symmetrize(#<lambda_CbDn_:3:2:4:0:{a b g h i c d e f}>,{g h i}),3), Symmetrize(Multiply(Gamma({g c}),#<lambda_BnCb_:2:0:3:2:{a b d e f h i}>),{c d e f},{g h i}))
//60
Add(Symmetrize(Multiply(Gamma({g a}),#<lambda_BbCn_:2:2:3:0:{b c h i d e f}>),{g h i},{a b c}), Symmetrize(Multiply(Gamma({g d}),#<lambda_BbCn_:2:2:3:0:{e f h i a b c}>),{d e f},{g h i}), Scale(Symmetrize(#<lambda_CnDb_:3:0:4:2:{d e f a b c g h i}>,{g h i}),2), Scale(Symmetrize(#<lambda_CnDb_:3:0:4:2:{a b c d e f g h i}>,{g h i}),2))
//62
Add(Symmetrize(Multiply(Gamma({i a}),#<lambda_CbDn_:3:2:4:0:{b c d j k e f g h}>),{a b c d},{i j k}), Symmetrize(Multiply(Gamma({i e}),#<lambda_CbDn_:3:2:4:0:{f g h j k a b c d}>),{e f g h},{i j k}))
