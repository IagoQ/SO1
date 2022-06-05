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
	{ok, ListenSocket} = gen_tcp:listen(8000, [list,{reuseaddr, true}]),
	wait_connect(ListenSocket, 0).

wait_connect(ListenSocket, N) ->
	{ok, Socket} = gen_tcp:accept(ListenSocket),
	spawn (fun () -> wait_connect (ListenSocket, N+1) end),
	get_request(Socket).


get_request(Socket) ->
	
	receive
		{tcp,Socket,Msg} -> 
			case parse_message(Msg) of 
				close -> gen_tcp:close(Socket);
				_ -> get_request(Socket)
			end;

		{newN,N} -> gen_tcp:send(Socket,[integer_to_list(N) ++ "\n", 0]),
					get_request(Socket);

		{error, _} -> closed
	end.


parse_message(Msg) ->
	case Msg of 
		"NUEVO\n" -> number_provider_pid ! {new, self()}, continue;
		"CHAU\n" -> close;
		_ -> continue
	end.