-module(turnos).
-export([server/0, number_provider/1]).

% entrypoint
server() ->
	Number_provider_pid = spawn(?MODULE, number_provider, [0]),
	register(number_provider_pid, Number_provider_pid),
	{ok, ListenSocket} = gen_tcp:listen(4040, [list,{reuseaddr, true}]),
	wait_connect(ListenSocket, 0).

% se encarga de mantener el numero actual  y proveer numeros
number_provider(N) ->
	receive
		{new, Pid} -> Pid ! {newN,N},
		number_provider(N+1);
		_ -> number_provider(N)
	end.

wait_connect(ListenSocket, N) ->
	{ok, Socket} = gen_tcp:accept(ListenSocket),
	spawn (fun () -> wait_connect (ListenSocket, N+1) end),
	get_request(Socket,"").


% recibe mensajes y
% acumula los mensajes en caso que sean parciales
get_request(Socket, PreexistingMsgs) ->
	receive
		{tcp,Socket,Msg} -> 
			Leftovers = parse_tokens(string:split(PreexistingMsgs ++ Msg, "\n")),
			get_request(Socket, Leftovers);

		{newN,N} -> gen_tcp:send(Socket,[integer_to_list(N) ++ "\n", 0]),
					get_request(Socket, PreexistingMsgs);

		{close} -> gen_tcp:close(Socket);
		{error, _} -> closed
	end.


% recibe una lista de tokens (mensajes separados por \n)
% procesa cada token y
% retorna solamente el ultimo token en el caso que sea parcial
parse_tokens([]) -> "";
parse_tokens([X]) -> case send_token(X) of 
						invalid -> X;
						_ -> ""
					end;
parse_tokens([X|Xs]) -> send_token(X),parse_tokens(Xs).



% logica para distintos tipos de mensaje
send_token(Msg) ->
	case Msg of 
		"NUEVO" -> number_provider_pid ! {new, self()}, valid;
		"CHAU" -> self() !{close}, valid;
		_ -> invalid
	end.