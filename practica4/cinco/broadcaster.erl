-module(broadcaster).
-export([server/0]).


server() ->
	loop([]).

loop(Subs) -> 
	receive 
		{send, Msg} -> broadcast(Msg,Subs),loop(Subs);
		{subscribe,Pid} -> loop([Pid|removePid(Pid,Subs)]);
		{unsubscribe,Pid} -> loop(removePid(Pid,Subs))
	end.

removePid(Pid,List) -> lists:filter(fun(Sub) -> Sub /= Pid end, List).

broadcast(Msg,[]) -> ok;
broadcast(Msg,[Sub|Subs]) -> Sub ! Msg, broadcast(Msg,Subs). 
