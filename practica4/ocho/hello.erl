-module(hello).
-export([init/0,hello/0]).
-on_load(reload/0).

hello() ->
    process_flag(trap_exit, true),
    spawn_link(?MODULE, printer, []),
    receive
	{'EXIT', From, Reason} -> spawn_link(?MODULE, printer, [])
    end,

printer() ->
	case whereis(hellopid) of 
		undefined -> register(printerpid, self());
		_ -> unregister(printerpid), register(printerpid, self())
	end,

	receive 
		upgrade -> 
			io:fwrite("calling \n"),
			spawn(?MODULE,printer,[]),
			exit("reloaded")
	after 1000 -> ok end,
	io:fwrite("2 ~p~n", [case rand:uniform(10) of 10 -> uno; _ -> self() end]),
	hello().

init() -> spawn(fun () -> hello() end).

reload() ->
	case whereis(printerpid) of 
		undefined -> ok;
		Pid -> io:fwrite("reloading \n"),Pid ! upgrade, ok
	end.
