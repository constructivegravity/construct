//Script to calculate the ptheory case
//(c) Jonas Schneider
//
//3
#<lambda_BnBa_:2:0:2:1:{a b c d e}>
//5
#<lambda_BnDa_:2:0:4:1:{a b c d e f g}>
//9
#<lambda_BaDn_:2:1:4:0:{e f g a b c d}>
//14
#<lambda_DnDa_:4:0:4:1:{a b c d e f g h i}>
//43a
Add(#<lambda_BnBa_:2:0:2:1:{a b p q c}>, Scale(Multiply(InverseGamma({m n}),#<lambda_BaDn_:2:1:4:0:{p q c a b m n}>),-6))
//43c
Add(#<lambda_BnDa_:2:0:4:1:{a b p q r s c}>, Scale(Multiply(InverseGamma({m n}),#<lambda_DnDa_:4:0:4:1:{a b m n p q r s c}>),-6))
//46a
Add(Symmetrize(#<lambda_BnBa_:2:0:2:1:{p q a b c}>,{b c}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_BnDa_:2:0:4:1:{p q m n a b c}>),{b c}),-6))
//46c
Add(Symmetrize(#<lambda_BaDn_:2:1:4:0:{a b c p q r s}>,{b c}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_DnDa_:4:0:4:1:{p q r s m n a b c}>),{b c}),-6))
