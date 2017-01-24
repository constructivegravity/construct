//Script to calculate the ptheory case
//(c) Jonas Schneider
//
//1a
Add(Scale(#<lambda_nnBb_:0:0:2:2:{p q a b}>,0.75), Scale(#<theta_BB_:2:0:2:0:{a b p q}>,-1), Scale(Multiply(InverseGamma({m n}),#<theta_BD_:2:0:4:0:{p q a b m n}>),6))
//1b
Add(Scale(#<lambda_nnDb_:0:0:4:2:{p q r s a b}>,0.75), Scale(#<theta_BD_:2:0:4:0:{a b p q r s}>,-1), Scale(Multiply(InverseGamma({m n}),#<theta_DD_:4:0:4:0:{p q r s a b m n}>),6))
//10
Add(Scale(#<lambda_BaBa_:2:1:2:1:{a b f c d e}>,0.25), Scale(#<lambda_BnBb_:2:0:2:2:{a b c d e f}>,-0.5), Scale(Symmetrize(Multiply(Gamma({e a}),#<theta_BB_:2:0:2:0:{b f c d}>),{a b}),-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({e a}),#<theta_BD_:2:0:4:0:{c d b m n f}>),{a b}),6), Scale(Multiply(Gamma({e f}),#<theta_BB_:2:0:2:0:{a b c d}>),0.5), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({e c}),#<xi_CBa_:3:0:2:1:{d m n a b f}>),{c d}),-0.75), Scale(#<xi_CBa_:3:0:2:1:{c d e a b f}>,-0.75), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({e a}),#<xi_BCa_:2:0:3:1:{c d m n b f}>),{a b}),-0.75), Scale(#<xi_BCa_:2:0:3:1:{c d a b e f}>,-0.75))
//12
Add(Scale(#<lambda_BaDa_:2:1:4:1:{a b h c d e f g}>,0.25), Scale(#<lambda_BnDb_:2:0:4:2:{a b c d e f g h}>,-0.5), Scale(Symmetrize(Multiply(Gamma({g a}),#<theta_BD_:2:0:4:0:{b h c d e f}>),{a b}),-1), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({g a}),#<theta_DD_:4:0:4:0:{b m n h c d e f}>),{a b}),6), Scale(Multiply(Gamma({g h}),#<theta_BD_:2:0:4:0:{a b c d e f}>),0.5), Scale(Symmetrize(Multiply(Gamma({g c}),#<xi_CBa_:3:0:2:1:{d e f a b h}>),{c d e f}),-0.25), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({g a}),#<xi_DCa_:4:0:3:1:{c d e f m n b h}>),{a b}),-0.75), Scale(#<xi_DCa_:4:0:3:1:{c d e f a b g h}>,-0.75))
//16
Add(#<lambda_CaCa_:3:1:3:1:{d e f g a b c h}>, Scale(#<lambda_CnCb_:3:0:3:2:{a b c d e f g h}>,-2), Scale(Multiply(Gamma({g h}),#<theta_CC_:3:0:3:0:{a b c d e f}>),2), Scale(Symmetrize(Multiply(Gamma({g d}),#<xi_BCa_:2:0:3:1:{e f a b c h}>),{d e f}),-2), Scale(#<xi_DCa_:4:0:3:1:{d e f g a b c h}>,-4), Scale(Symmetrize(Multiply(Gamma({g a}),#<xi_CBa_:3:0:2:1:{d e f b c h}>),{a b c}),-2), Scale(#<xi_CDa_:3:0:4:1:{d e f a b c g h}>,-4))
//18
Add(#<lambda_BaDa_:2:1:4:1:{e f g a b c d h}>, Scale(#<lambda_BbDn_:2:2:4:0:{e f g h a b c d}>,-2), Scale(Multiply(Gamma({g h}),#<theta_BD_:2:0:4:0:{e f a b c d}>),2), Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({g e}),#<xi_CDa_:3:0:4:1:{f m n a b c d h}>),{e f}),-3), Scale(#<xi_CDa_:3:0:4:1:{e f g a b c d h}>,-3), Scale(Symmetrize(Multiply(Gamma({g a}),#<xi_BCa_:2:0:3:1:{e f b c d h}>),{a b c d}),-1))
//20
Add(#<lambda_DaDa_:4:1:4:1:{a b c d j e f g h i}>, Scale(#<lambda_DnDb_:4:0:4:2:{a b c d e f g h i j}>,-2), Scale(Multiply(Gamma({i j}),#<theta_DD_:4:0:4:0:{a b c d e f g h}>),2), Scale(Symmetrize(Multiply(Gamma({i e}),#<xi_CDa_:3:0:4:1:{f g h a b c d j}>),{e f g h}),-1), Scale(Symmetrize(Multiply(Gamma({i a}),#<xi_DCa_:4:0:3:1:{e f g h b c d j}>),{a b c d}),-1))
//27
Add(Symmetrize(#<xi_BCa_:2:0:3:1:{d e a b c f}>,{e f}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<xi_DCa_:4:0:3:1:{m n d e a b c f}>),{e f}),-6), Scale(Symmetrize(Multiply(Gamma({d a}),#<lambda_nnBb_:0:0:2:2:{b c e f}>),{a b c}),1.5), Scale(#<lambda_nnDb_:0:0:4:2:{a b c d e f}>,3))
//36
Add(Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({f a}),#<theta_CC_:3:0:3:0:{b m n c d e}>),{a b}),1.5), Scale(#<theta_CC_:3:0:3:0:{a b f c d e}>,1.5), Symmetrize(Multiply(Gamma({f c}),#<theta_BB_:2:0:2:0:{d e a b}>),{c d e}), Scale(#<theta_BD_:2:0:4:0:{a b c d e f}>,2), Scale(#<xi_CBa_:3:0:2:1:{c d e a b f}>,-0.25), Scale(#<xi_BCa_:2:0:3:1:{a b c d e f}>,-0.25))
//39
Add(Scale(Symmetrize(Multiply(Gamma({h d}),#<theta_CC_:3:0:3:0:{e f g a b c}>),{d e f g}),0.5), Symmetrize(Multiply(Gamma({h a}),#<theta_BD_:2:0:4:0:{b c d e f g}>),{a b c}), Scale(#<theta_DD_:4:0:4:0:{d e f g a b c h}>,2), Scale(#<xi_CDa_:3:0:4:1:{a b c d e f g h}>,-0.25), Scale(#<xi_DCa_:4:0:3:1:{d e f g a b c h}>,-0.25))
//44a
Add(Scale(Multiply(Gamma({a b}),#<lambda_nnBb_:0:0:2:2:{p q c d}>),1.5), Scale(Symmetrize(Multiply(Gamma({a c}),#<lambda_nnBb_:0:0:2:2:{p q d b}>),{c d}),-3), Scale(#<lambda_BnBb_:2:0:2:2:{a b p q c d}>,-1), Scale(Multiply(InverseGamma({m n}),#<lambda_BbDn_:2:2:4:0:{p q c d m n a b}>),6), Scale(Symmetrize(Multiply(Gamma({b p}),#<lambda_nnBb_:0:0:2:2:{q a c d}>),{p q}),3))
//44c
Add(Scale(Multiply(Gamma({a b}),#<lambda_nnDb_:0:0:4:2:{p q r s c d}>),1.5), Scale(Symmetrize(Multiply(Gamma({a c}),#<lambda_nnDb_:0:0:4:2:{p q r s d b}>),{c d}),-3), Scale(#<lambda_BnDb_:2:0:4:2:{a b p q r s c d}>,-1), Scale(Multiply(InverseGamma({m n}),#<lambda_DnDb_:4:0:4:2:{m n a b p q r s c d}>),6), Scale(Symmetrize(Multiply(Gamma({b p}),#<lambda_nnDb_:0:0:4:2:{q r s a c d}>),{p q r s}),6))
//47a
Add(Scale(Multiply(Gamma({a b}),#<lambda_nnBb_:0:0:2:2:{p q c d}>),1.5), Symmetrize(#<lambda_BaBa_:2:1:2:1:{a c d p q b}>,{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_BaDa_:2:1:4:1:{p q b m n a c d}>),{c d}),-6), Scale(Symmetrize(Multiply(Gamma({c p}),#<lambda_nnBb_:0:0:2:2:{q a d b}>),{p q},{c d}),-6))
//47c
Add(Scale(Multiply(Gamma({a b}),#<lambda_nnDb_:0:0:4:2:{p q r s c d}>),1.5), Symmetrize(#<lambda_BaDa_:2:1:4:1:{a c d p q r s b}>,{c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_DaDa_:4:1:4:1:{m n a c d p q r s b}>),{c d}),-6), Scale(Symmetrize(Multiply(Gamma({c p}),#<lambda_nnDb_:0:0:4:2:{q r s a d b}>),{p q r s},{c d}),-12))
//49
Add(Symmetrize(#<lambda_nnBb_:0:0:2:2:{a b c d}>,{b c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_nnDb_:0:0:4:2:{m n a b c d}>),{b c d}),-6))
//50a
Add(Symmetrize(#<lambda_BnBb_:2:0:2:2:{p q a b c d}>,{b c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_BnDb_:2:0:4:2:{p q m n a b c d}>),{b c d}),-6), Scale(Symmetrize(Multiply(Gamma({b p}),#<lambda_nnBb_:0:0:2:2:{q a c d}>),{b c d},{p q}),-3))
//50c
Add(Symmetrize(#<lambda_BbDn_:2:2:4:0:{a b c d p q r s}>,{b c d}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<lambda_DnDb_:4:0:4:2:{p q r s m n a b c d}>),{b c d}),-6), Scale(Symmetrize(Multiply(Gamma({b p}),#<lambda_nnDb_:0:0:4:2:{q r s a c d}>),{b c d},{p q r s}),-6))
//53b
Add(Symmetrize(#<xi_CBa_:3:0:2:1:{p q r a b c}>,{b c}), Scale(Symmetrize(Multiply(InverseGamma({m n}),#<xi_CDa_:3:0:4:1:{p q r m n a b c}>),{b c}),-6))
//55
Add(Symmetrize(Multiply(Gamma({d a}),#<lambda_nnBb_:0:0:2:2:{b c e f}>),{a b c},{d e f}), Scale(Symmetrize(#<lambda_nnDb_:0:0:4:2:{a b c d e f}>,{d e f}),2))
//58
Add(Scale(Symmetrize(Multiply(InverseGamma({m n}),Gamma({f a}),#<lambda_CnCb_:3:0:3:2:{c d e b m n g h}>),{f g h},{a b}),0.75), Scale(Symmetrize(#<lambda_CnCb_:3:0:3:2:{c d e a b f g h}>,{f g h}),0.75), Scale(Symmetrize(Multiply(Gamma({f c}),#<lambda_BnBb_:2:0:2:2:{a b d e g h}>),{c d e},{f g h}),0.5), Symmetrize(#<lambda_BnDb_:2:0:4:2:{a b c d e f g h}>,{f g h}), Scale(Symmetrize(Multiply(Gamma({f a}),#<lambda_nnDb_:0:0:4:2:{b c d e g h}>),{f g h},{a b}),-3))
//61
Add(Scale(Symmetrize(Multiply(Gamma({h a}),#<lambda_BbDn_:2:2:4:0:{b c i j d e f g}>),{h i j},{a b c}),2), Scale(Symmetrize(#<lambda_DnDb_:4:0:4:2:{d e f g a b c h i j}>,{h i j}),4), Symmetrize(Multiply(Gamma({h d}),#<lambda_CnCb_:3:0:3:2:{a b c e f g i j}>),{d e f g},{h i j}))
