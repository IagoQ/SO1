#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

/*
 * Para probar, usar netcat. Ej:
 *
 *      $ nc localhost 4040
 *      NUEVO
 *      0
 *      NUEVO
 *      1
 *      CHAU
 */

void quit(char *s)
{
	perror(s);
	abort();
}

int U = 0;
pthread_mutex_t mutex;

int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;

	/*
	 * Leemos de a un caracter (no muy eficiente...) hasta
	 * completar una línea.
	 */
	while ((rc = read(fd, buf + i, 1)) > 0) {
		if (buf[i] == '\n')
			break;
		i++;
	}

	if (rc < 0)
		return rc;

	buf[i] = 0;
	return i;
}

void handle_conn(int csock)
{
	char buf[200];
	int rc;

	while (1) {
		/* Atendemos pedidos, uno por linea */
		rc = fd_readline(csock, buf);
		if (rc < 0)
			quit("read... raro");

		if (rc == 0) {
			/* linea vacia, se cerró la conexión */
			close(csock);
			return;
		}


		if (!strcmp(buf, "NUEVO")) {
			char reply[20];

      // la zona critica es solamente el contador, lectura y inc 
      pthread_mutex_lock(&mutex);
			sprintf(reply, "%d\n", U);
			U++;
      pthread_mutex_unlock(&mutex);

			write(csock, reply, strlen(reply));
		} else if (!strcmp(buf, "CHAU")) {
			close(csock);
			return;
		}
	}
}

void *handle_conn_wrapper(void *arg){
  int sock = *( (int*) arg);
  handle_conn(sock);
  return NULL;
}

void concurrent_handle_conn(int csock){
  pthread_t t;
	pthread_create(&t,NULL,handle_conn_wrapper, (void*)&csock);
  return;
}

void wait_for_clients(int lsock)
{
	int csock;

	/* Esperamos una conexión, no nos interesa de donde viene */
	csock = accept(lsock, NULL, NULL);
	if (csock < 0)
		quit("accept");

	/* Atendemos al cliente */
  pthread_t t;
	pthread_create(&t,NULL,handle_conn_wrapper, (void*)&csock);
  // handle_conn(&csock);
  
  // DUDA: si lo implemento de esta manera (para no mezclar la logica de lanzar los pthreads), al castear el int a void* e int devuelta cambia el valor, no me estoy dando cuenta pq
  // concurrent_handle_conn(csock);


	/* Volvemos a esperar conexiones */
	wait_for_clients(lsock);
}

/* Crea un socket de escucha en puerto 4040 TCP */
int mk_lsock()
{
	struct sockaddr_in sa;
	int lsock;
	int rc;
	int yes = 1;

	/* Crear socket */
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0)
		quit("socket");

	/* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(4040);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto 4040 TCP, en todas las direcciones disponibles */
	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	rc = listen(lsock, 10);
	if (rc < 0)
		quit("listen");

	return lsock;
}

int main()
{
	int lsock;
	lsock = mk_lsock();

	pthread_mutex_init(&mutex,NULL);

	wait_for_clients(lsock);
}
