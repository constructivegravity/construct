// Script file to calculate the metric case
// (c) Jonas Schneider

// C1
Add(#<lambda:0:0:2:2:{a b c d}>,Scale(#<theta:2:0:2:0:{c d a b}>,-4))

// C2
#<lambda:0:0:2:1:{a b c}>
#<lambda:2:0:2:1:{a b c d e}>
Add(#<lambda:2:1:2:1:{a b e c d f}>,Scale(#<lambda:2:0:2:2:{c d a b e f}>,-2),Scale(Symmetrize(Multiply(Gamma({e c}),#<theta:2:0:2:0:{d f a b}>),{c d}),-4),Scale(Multiply(Gamma({e f}),#<theta:2:0:2:0:{c d a b}>),2))

// C10
Add(Multiply(Gamma({a b}),#<lambda:0:0:0:0:{}>),Scale(#<lambda:0:0:2:0:{a b}>,-2))
Add(Multiply(Gamma({c d}),#<lambda:0:0:2:0:{a b}>),Scale(Symmetrize(Multiply(Gamma({c a}),#<lambda:0:0:2:0:{b d}>),{a b}),2),Scale(#<lambda:2:0:2:0:{c d a b}>,-2))
Add(Multiply(Gamma({d e}),#<lambda:0:0:2:1:{a b c}>),Scale(Multiply(Gamma({c e}),#<lambda:0:0:2:1:{a b d}>),-1),Scale(#<lambda:2:0:2:1:{d e a b c}>,-2),Scale(Symmetrize(Multiply(Gamma({d a}),#<lambda:0:0:2:1:{b e c}>),{a b}),2))
Add(Multiply(Gamma({e f}),#<lambda:0:0:2:2:{a b c d}>),Scale(Symmetrize(Multiply(Gamma({f c}),#<lambda:0:0:2:2:{a b d e}>),{c d}),-2),Scale(#<lambda:2:0:2:2:{e f a b c d}>,-2),Scale(Symmetrize(Multiply(Gamma({e a}),#<lambda:0:0:2:2:{b f c d}>),{a b}),2))

// C11
Symmetrize(#<lambda:0:0:2:1:{a b c}>,{b c})
Add(Symmetrize(Multiply(Gamma({e a}),#<lambda:0:0:2:1:{b c d}>),{e d},{a b}),Scale(Symmetrize(#<lambda:2:0:2:1:{a b c d e}>,{d e}),-1))
Add(Multiply(Gamma({c d}),#<lambda:0:0:2:2:{a b e f}>),Scale(Symmetrize(Multiply(Gamma({e a}),#<lambda:0:0:2:2:{b c f d}>),{e f},{a b}),-4),Scale(Symmetrize(#<lambda:2:1:2:1:{a b d c e f}>,{e f}),2))
Symmetrize(#<lambda:0:0:2:2:{a b c d}>,{b c d})
Add(Symmetrize(#<lambda:2:0:2:2:{a b c d e f}>,{d e f}),Scale(Symmetrize(Multiply(Gamma({d a}),#<lambda:0:0:2:2:{b c e f}>),{d e f},{a b}),-1))