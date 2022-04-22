# El Problema de los Fumadores

## Deadlock
Puede ocurrir que uno de los otros fumadores que no pueden fumar según los ingredientes que ha dejado el agente le quita al que podría fumar uno de los ingredientes que necesita. Por ejemplo, un caso de deadlock sería el caso de que el agente deje en la mesa los ingredientes de tabaco y papel y así el fumador que podría fumar sería el 1 pero si el fumador 3 es más rápido y se ejecuta antes tomando el papel, el fumador 1 se quedaría esperando a tomar el papel y el fumador 3 también queda esperando ya que el agente no dejo fósforos. Además como ninguno fuma el agente no dará nuevos ingredientes.

## Solución
Una solución es que cada fumador tome un lock (para que ninguno de los otros fumadores pueda sacarle un ingrediente), verifique que ambos ingredientes que necesita estén disponibles y, en caso de estarlos, fumar. En el caso de que no estén ambos disponibles espera a que el agente agregue algún nuevo ingrediente y vuelve a verificar si tiene lo necesario para fumar.
Los consumidores estarán esperando nuevos ingredientes y cuando el agente ponga a disposición alguno, dependiendo del ingrediente, un consumidor lo agarra y despierta a los fumadores para que verifiquen si el ingrediente les sirve. De esta manera nunca se producirá un deadlock.