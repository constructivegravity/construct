# Construct

Software that allows to solve construction equations that determine the Langrangian of (so far linearized) physical theories.

## Features

 * Interactive console
 * Generate tensors build from the totally antisymmetric tensor and a background metric
 * Apply symmetries to tensors
 * Determine the linear independent terms

## Example

```
Arbitrary({\alpha \beta \gamma \delta})
#=> e_1 * \gamma_{\alpha\beta}\gamma_{\gamma\delta}  +
    e_2 * \gamma_{\alpha\gamma}\gamma_{\beta\delta}  +
    e_3 * \gamma_{\alpha\delta}\gamma_{\beta\gamma}
Symmetrize(%, {\alpha \beta}, {\gamma \delta})
# => e_1 * \gamma_{\alpha\beta}\gamma_{\gamma\delta}  +
     e_2 * 0.5*(\gamma_{\alpha\gamma}\gamma_{\beta\delta} + \gamma_{\beta\gamma}\gamma_{\alpha\delta})
```

## Disclaimer

(c) 2016 Constructive Gravity Group Erlangen

For private and educational use only.
