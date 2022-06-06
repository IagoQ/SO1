-module(seis).
-export([createLock/0, lock/1, unlock/1, destroyLock/1, lockLoop/0]).
-export([createSem/1, semP/1, semV/1, destroySem/1, semLoop/1]).
-export([testLock/0, testSem/0]).

createLock () -> spawn(?MODULE, lockLoop, []).
lock (L) -> L ! {lock, self()}, receive ok -> ok end. %espero a recibir ok
unlock (L) -> L ! unlock.
destroyLock (L) -> L ! destroy.

lockLoop()->
    receive
        {lock, Pid} -> Pid ! ok, %tomo el lock y envio ok
        receive 
            unlock -> lockLoop() %espero a que suelten el lock
        end;
             
        destroy -> ok %termina el loop

    end.

createSem (N) -> spawn(?MODULE, semLoop, [N]).
semP (S) -> S ! {wait, self()}, receive ok -> ok end. %espero a recibir ok
semV (S) -> S ! post.
destroySem (S) -> S ! destroy.

semLoop(N)->
    receive
        post -> semLoop(N + 1); %incremento el contador

        {wait, Pid}->
            if 
                (N == 0) -> receive post -> Pid ! ok end, semLoop(N); %si el contador esta en cero espero a que hagan un post y ya lo consumo
                true -> Pid ! ok, semLoop(N - 1) %sino decremento el contador
            end;
        
        destroy -> ok %termina el loop

    end.

f (L, W) ->
    lock(L),
    % regioncritica(),
    io:format("uno ~p~n", [self()]),
    io:format("dos ~p~n", [self()]),
    io:format("tre ~p~n", [self()]),
    io:format("cua ~p~n", [self()]),
    unlock(L),
    W ! finished.

waiter (L, 0) -> destroyLock(L);
waiter (L, N) -> receive finished -> waiter(L, N-1) end.

waiter_sem (S, 0) -> destroySem(S);
waiter_sem (S, N) -> receive finished -> waiter_sem(S, N-1) end.

testLock () ->
    L = createLock(),
    W = spawn(fun () -> waiter(L, 3) end),
    spawn (fun () -> f(L, W) end),
    spawn (fun () -> f(L, W) end),
    spawn (fun () -> f(L, W) end),
    ok.

sem (S, W) ->
    semP(S),
    %regioncritica(), bueno, casi....
    io:format("uno ~p~n", [self()]),
    io:format("dos ~p~n", [self()]),
    io:format("tre ~p~n", [self()]),
    io:format("cua ~p~n", [self()]),
    io:format("cin ~p~n", [self()]),
    io:format("sei ~p~n", [self()]),
    semV(S),
    W ! finished.

testSem () ->
    S = createSem(2), % a lo sumo dos usando io al mismo tiempo
    W = spawn (fun () -> waiter_sem (S, 5) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    ok.