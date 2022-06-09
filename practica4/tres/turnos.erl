-module(turnos).
-export([server/0, number_provider/1]).

number_provider(N) ->
	receive
		{new, Pid} -> Pid ! {newN,N},
		number_provider(N+1);
		_ -> number_provider(N)
	end.


server() ->
	Number_provider_pid = spawn(?MODULE, number_provider, [0]),
	register(number_provider_pid, Number_provider_pid),
	{ok, ListenSocket} = gen_tcp:listen(8002, [list,{reuseaddr, true}]),
	wait_connect(ListenSocket, 0).

wait_connect(ListenSocket, N) ->
	{ok, Socket} = gen_tcp:accept(ListenSocket),
	spawn (fun () -> wait_connect (ListenSocket, N+1) end),
	get_request(Socket,"").


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



parse_tokens([]) -> "";
parse_tokens([X]) -> case send_token(X) of 
						invalid -> X;
						_ -> ""
					end;
parse_tokens([X|Xs]) -> send_token(X),parse_tokens(Xs).



send_token(Msg) ->
	case Msg of 
		"NUEVO" -> number_provider_pid ! {new, self()}, valid;
		"CHAU" -> self() !{close}, valid;
		_ -> invalid
	end.