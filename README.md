# rgb asm
interpréteur pour programmer un [module de LEDs RGB hexagonal](https://shop.m5stack.com/products/hex-rgb-led-board-sk6812)
Utilisable pour n'importe quelle librairie de LED adressable en Arduino, avec quelques changements

## description
256 octets de mémoire. il n'y a pas de registres, tout se fait en RAM ! (vu que c'est une VM)
interface en 8-bit `uint8_t`, interne en `int`

certaines valeurs peuvent être *immediate* ou être des **pointeurs**: une adresse mémoire.
Dans ce cas, on rajoute un 'i' à la fin. (oups c'est inversé ^^)

Les arguments sont séparés par des virgules

## liste de instructions
* `LED <number|pointer>,<red|pointer>,<green|pointer>,<blue|pointer>[i]`
* `JMP [-]<length|pointer>[i][c]` +n ou -n, le `c` indique un jump conditionnel
* `CMP <address>,<value|pointer>[i]` i: inversé supérieur au lieu d'inférieur 
* `STORE <address>,<value|pointer>[i]`
* `ADD <address>,<value|pointer>[i]`
* `SUB <address>,<value|pointer>[i]`
* `MUL <address>,<value|pointer>[i]`
* `DBG <address>` : affiche la valeur
* `SLEEP <value|pointer>[i]` délai de x centièmes de secondes (100ms)
* `SHOW _` met à jour l'affichage des LEDs
* `RAND <address>` nombre aléatoire dans la case mémoire
* `LNJMP <len>` : saut long relatif, sans pointeurs (hardcodé)
* `ABSJMP <address>`: saut absolu hardcodé

## Exemple
```asm
STORE 1,0
ADD 1,1
CMP 1,10
JMP 1c
DBG 1
LED 0,255,255,255
```