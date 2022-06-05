-module(test).
-export([test/0]).
-import(client, [client/0]).
-import(broadcaster, [server/0]).



test() -> 
	BPid = spawn(broadcaster,server,[]),
	subscribe(BPid,10),
	BPid ! {send, "ji"}.



subscribe(Broadcaster_pid, 0) -> ok;
subscribe(Broadcaster_pid,N) -> 
	CPid = spawn(client,client,[]),
	Broadcaster_pid ! {subscribe, CPid},
	subscribe(Broadcaster_pid,N-1).
