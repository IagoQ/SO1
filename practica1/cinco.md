# Cena de los Filósofos

## Deadlock
Puede ocurrir que los N_FILOSOFOS tomen solo el tenedor de su derecha primero y así cuando alguno intente tomar el de la izquierda no habrá tenedores disponibles. Además como ninguno empezara a comer tampoco soltaran el tenedor derecho y todos quedan esperando.

## Solución 1
Si algún filosofo fuese zurdo no se podría dar la situación anterior ya que uno tomara primero el tenedor izquierdo y cuando el resto vaya a tomar el tenedor derecho todos menos uno podrán hacerlo. El que este a la izquierda del zurdo deberá esperar a que este termine y esto pasara ya que alguno del resto de los filósofos pudo agarrar los dos tenedores y empezó a comer.

## Solución 2
Si hay N-1 filósofos seguro que al menos uno puede tomar los dos tenedores y en algún momento dejara de comer dejando los tenedores y comenzara a comer otro. 