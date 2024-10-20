# Práctica 3 - SE 24/25

Implementa un pequeno xogo coa placa: o xogo consiste en que o usuario
ten que premer no botón correspondente ao LED que a placa teña acendido
en cada momento.

- O botón esquerdo (sw2/sw3) correponde á luz vermella (luz esquerda)
  e o botón dereito (sw1) á luz dereita.

- A xestión da E/S cos botóns farase con interrupcións.

- Só unha das luces pode estar acendida en cada momento. A secuencia de
  acendidos podería aleatorizarse, pero imos facelo máis simple,
  considerando unha secuencia de bits fixa (que se pode mudar: variable
  'sequence' no código)

- Cóntanse os acertos e os erros durante a secuencia completa, e vanse
  actualizando na pantalla LCD con este formato: hits:misses

- Ao rematar a secuencia non se admiten máis pulsacións, e o resultado
  final (que xa non se actualizará) móstrase pestanexando no LCD,
  mantendo o mesmo formato.
