//Script to calculate the ptheory case
//(c) Jonas Schneider
//
//23
Add(Symmetrize(Multiply(Gamma({d a}),#<lambda_nnBn_:0:0:2:0:{b c}>),{a b c}), Scale(#<lambda_nnDn_:0:0:4:0:{a b c d}>,2))
//30
Add(Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({f a}),#<lambda_CnCn_:3:0:3:0:{b m n c d e}>),{a b}),0.75), Scale(#<lambda_CnCn_:3:0:3:0:{a b f c d e}>,0.75), Scale(Symmetrize(Multiply(Gamma({f c}),#<lambda_BnBn_:2:0:2:0:{d e a b}>),{c d e}),0.5), #<lambda_BnDn_:2:0:4:0:{a b c d e f}>, Scale(Symmetrize(Multiply(Gamma({f a}),#<lambda_nnDn_:0:0:4:0:{b c d e}>),{a b}),-3))
//33
Add(Symmetrize(Multiply(Gamma({h d}),#<lambda_CnCn_:3:0:3:0:{e f g a b c}>),{d e f g}), Scale(Symmetrize(Multiply(Gamma({h a}),#<lambda_BnDn_:2:0:4:0:{b c d e f g}>),{a b c}),2), Scale(#<lambda_DnDn_:4:0:4:0:{d e f g a b c h}>,4))
//41
Add(Scale(Multiply(Gamma({a b}),#<lambda_nnnn_:0:0:0:0:{}>),1.5), Scale(#<lambda_nnBn_:0:0:2:0:{a b}>,-1), Scale(Multiply(InverseGamma({m n}),#<lambda_nnDn_:0:0:4:0:{a b m n}>),6))
//42a
Add(Scale(Multiply(Gamma({a b}),#<lambda_nnBn_:0:0:2:0:{p q}>),1.5), Scale(#<lambda_BnBn_:2:0:2:0:{a b p q}>,-1), Scale(Symmetrize(Multiply(Gamma({b p}),#<lambda_nnBn_:0:0:2:0:{q a}>),{p q}),3), Scale(Multiply(InverseGamma({m n}),#<lambda_BnDn_:2:0:4:0:{p q a b m n}>),6))
//42b
Add(Scale(Multiply(Gamma({a b}),#<lambda_nnDn_:0:0:4:0:{p q r s}>),1.5), Scale(#<lambda_BnDn_:2:0:4:0:{a b p q r s}>,-1), Scale(Symmetrize(Multiply(Gamma({b p}),#<lambda_nnDn_:0:0:4:0:{q r s a}>),{p q r s}),6), Scale(Multiply(InverseGamma({m n}),#<lambda_DnDn_:4:0:4:0:{a b m n p q r s}>),6))
