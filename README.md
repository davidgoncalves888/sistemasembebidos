# Práctica 4 - SE 24/25

## Parte 1

La función se llama My_Div2, en la carpeta drivers_EMB está la implementación embebida, en la drivers_ENS está la
implementación con el archivo My_Div2.s.

### Para compilar:

```
`make flash_embebido`
```

```
`make flash_ensamblar`
```

## Parte 2

Buscando en internet he visto que hay varios algoritmos para invertir los bits, habiendo dos muy populares:

- el que invierte dividiendo en 2, 4, 8, 16 bits y moviendo los bits para darle la vuelta,
- y el que usando una tabla hash busca 8 bits y su contrario.
  El primero es el más conveniente en situaciones de poca memoria, aunque puede ser un poco más lento que el segundo,
  que en caso de tener que hacer esta operación muchas veces puede ser mejor. Aunque algunas arquitecturas incluyen esta
  función de base, como algunos ARM (RBIT, y en RISC-V con la extension B (brev8).
  En este caso veo mejor el primer algoritmo por la poca memoria que usa, más apto si fuera implementado en un aplaca
  como la de prácticas. El código está en reverse_int.s.
