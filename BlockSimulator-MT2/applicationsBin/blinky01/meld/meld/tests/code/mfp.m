meld fil    
   .                                         	          +      init -o axioms  initialize() -o may-do-init-height(), {B, W, T | !edge(B, W, T), 
			T = 1 | rv(B, float(W))}, {B, W, T | !edge(B, W, T), 
			T = 0 | rv(B, 0)}, {B, W, T | !edge(B, W, T) | 
			edge-height(B, 0)}, [ COUNT => Outbound,  | B, W, Out | !edge(B, W, Out), 
			Out = 1 | !outbound(Outbound)].F   new-height(W, H), !token(), edge-height(W, MV57) -o edge-height(W, H).@   may-do-init-height(), !typ(MV63), MV63 = 1 -o do-init-height(0).'   proc-return(MV64), MV64 = 0 -o token().=   may-do-init-height(), !typ(NotSink), NotSink != 1 -o token().�   do-init-height(Ret), !height(Height) -o proc-return(Ret), {B, W, R, T | 
			!edge(B, W, T), rv(B, R), R = 0 | init-height(host-id, Height)@B, 
			rv(B, R)}, {B, W, R, T | !edge(B, W, T), rv(B, R), R != 0 | 
			new-height(host-id, Height)@B, rv(B, R)}.^   init-height(W, H), token(), edge-height(W, MV58) -o edge-height(W, H), 
			init-height2(W, H).L   init-height2(W, H), !typ(NotSource), NotSource != 0 -o init-height-check(H).r   init-height-check(H), height(Height), (Height = 0) NIL (Height > (H + 1)) -o height(H + 1), 
			do-init-height(1).'   proc-return(MV65), MV65 = 1 -o token().    init-height-check(H) -o token().z   init-height2(W, H), !typ(MV66), nbInitHeightMsgs(IH), MV66 = 0 -o 
			nbInitHeightMsgs(IH + 1), init-height2-check-arcs().r   init-height2-check-arcs(), !nbInitHeightMsgs(Total), !outbound(Out), Out = Total -o 
			init-height3-check-arcs().a   init-height2-check-arcs(), !nbInitHeightMsgs(Total), !outbound(Out), Out != Total -o 
			token().`   init-height3-check-arcs(), height(Height), excess(Excess) -o height(world), 
			change-excess().M   proc-return(MV67), state(State), MV67 = 3 -o state(0), 
			do-init-height(2).*   proc-return(MV68), MV68 = 2 -o do-push(0).^   change-excess() -o accumulate-excess(), {B, W | rv(B, W) | 
			rv-copy(B, W), rv-copy2(B, W)}.�   accumulate-excess() -o proc-return(3), {B, W | rv-copy(B, W) | 
			rv(B, W)}, [ SUM => W,  | B | rv-copy2(B, W) | 
			excess(W)].Z   do-push(MV69), !typ(MV70), !excess(Excess), MV69 = 1, 
			MV70 = 1 -o may-do-lift(Excess).=   do-push(MV71), !typ(MV72), MV71 = 0, MV72 = 1 -o 
			token().H   do-push(Type), !typ(NotSink), NotSink != 1 -o push-check-excesses(Type).  !push-check-excesses(Type), excess(Excess), !edge-height(U, EH), rv(U, RV), 
			!height(H), !edge(U, MV59, MV60), Delta = if (Excess < RV) then Excess else RV end, Excess > 0, RV > 0, 
			EH < H -o excess(Excess - Delta), rv(U, RV - Delta), push-request(host-id, Delta)@U./   push-check-excesses(MV73), MV73 = 0 -o token().L   push-check-excesses(MV74), !excess(Excess), MV74 = 1 -o may-do-lift(Excess).�   push-request(W, Delta), token(), !edge-height(W, EH), !height(H), 
			rv(W, RV), EH > H -o rv(W, RV + Delta), find-all-push-requests(Delta).�   find-all-push-requests(Acc), push-request(W, Delta), !edge-height(W, EH), !height(H), 
			rv(W, RV), EH > H -o rv(W, RV + Delta), find-all-push-requests(Acc + Delta).2   find-all-push-requests(Acc) -o push-request2(Acc).�   push-request(W, Delta), !edge-height(W, EH), !height(H), !edge(W, MV61, MV62), 
			EH <= H -o push-request-ans(host-id, Delta)@W, token().o   push-request2(Delta), state(MV75), !typ(NotSink), NotSink != 1, 
			MV75 = 1 -o state(0), push-request3(Delta).-   push-request2(Delta) -o push-request3(Delta).U   push-request3(Delta), excess(Excess) -o excess(Excess + Delta), push-request4(Delta).6   push-request4(Delta), !typ(MV76), MV76 = 1 -o token().@   push-request4(Delta), !typ(NotSink), NotSink != 1 -o do-push(1).�   push-request-ans(W, Delta), excess(Excess), state(State), rv(W, RV) -o 
			excess(Excess + Delta), state(0), rv(W, RV + Delta), do-push(1).'   may-do-lift(Val), Val > 0 -o do-lift().&   may-do-lift(Val), Val <= 0 -o token().8   do-lift(), !typ(MV77), MV77 = 2 -o select-best-height().   do-lift() -o token().�   select-best-height(), !excess(Excess) -o minimize-height(), {B, W, H | 
			rv(B, W), edge-height(B, H), W >= Excess | rv(B, W), 
			edge-height(B, H), to-select(H)}.N   minimize-height() -o [ MIN => H,  |  | to-select(H) | 
			lift-min-height(H)].�   lift-min-height(H), height(OldHeight) -o height(H + 1), do-push(0), 
			{B, W, Out | !edge(B, W, Out) | new-height(host-id, H + 1)@B}.       �   0        0      0      0       0      0       0      0      0       0    	   0   
   0      0      �              _init                                                               set-priority                                                        setcolor                                                             setedgelabel                                                         write-string                                                        add-priority                                                         schedule-next                                                       setColor2                                                            typ                                                                 state                                                               height                                                              excess                                                               edge                                                                nbInitHeightMsgs                                                    rv                                                                  rv-copy                                                             rv-copy2                                                             initialize                                                          init-height                                                         new-height                                                          edge-height                                                         init-height-check                                                   init-height2                                                         init-height2-check-arcs                                              init-height3-check-arcs                                              change-excess                                                        outbound                                                            do-push                                                              accumulate-excess                                                   push-request                                                        find-all-push-requests                                              push-request2                                                       push-request3                                                       push-request4                                                       push-request-ans                                                    push-check-excesses                                                 may-do-lift                                                          do-lift                                                             to-select                                                            select-best-height                                                   minimize-height                                                     lift-min-height                                                      may-do-init-height                                                   token                                                               proc-return                                                         do-init-height                                                           �                                             +   T      �  N   �
�        W   �   f  �  u  @ 0     0    0      @ 0     0    0      @ 0       y  @ 0     0    0      @ 0     0    0      @ 0      0    0      @ 0         @ 0     0    0      @ 0     0
    0      @ 0     0    0      @ 0      0    0      @ 0       j  @ 0     0    0      @ 0     0    0      @ 0     0
    0      @ 0     0    0      @ 0       �   @ 0     0    0      @ 0     0    0      @ 0     0    0      @ 0       [   @ 0     0    0      @ 0     0    0      @ 0           @
 0        @ 0             @	 0       @ 0        @   0 � 0         �           �    �0 @*:   �  4    �0!�"   `   @0  	�@   �  :    �0!�"   `!   @0  0         �+   �  %    �0!@0  0    �0 !        :   �  )    �0"�#   `   �!!   �@0! � �     L      �+  F    �0 � 8    �0!� *    B 0"@0  0���     +F      �* @    �0 �  2    �0!�"    `   @-0	    � �     */      �, )    �0 �!  	   `   @+� �     ,=      �* 7    �0 �  )    �0!�"    `   @+� �     *     �- �    �0 �
  �    �0!@,0   g   �  a    �0"� S    B 0#� $        `4   @0 0 0% @0  0��g   �  a    �0"� S    B 0#� $         `4   @0 0 0% @0  0��� �     
-b      �+ \    �0 � N    �0!� @    B 0"@0  0@0  0��� �     +D      � >    �0 �  0    �0!�"     `   @0  � �     r   	   � l    �0 �
 ^    �0!�"     �#     �#$	 �"$%`.   @
�      @-0
    �� �     
/   
   �, )    �0 �!  
   `   @+� �     ,      �     �0 @+� �     `      � Z    �0 �  L    �0!�"     `3   � -    �0"@�     @�� �     I      � C    �0 �  5    �0!�  '    �0"�#  `   @� �     I      � C    �0 �  5    �0!�  '    �0"�#  `   @+� �     M      � G    �0 �
 9    �0!� +    �0"@
0    @��� �     
W      �, Q    �0 �!     `8   �	 2    �0!@	0    @-0    �� �     	,8      �, 2    �0 �!     `   @0    � �     ,]      � W    �0 @?   � 9    �0!@0  0@0  0��� �     �      � �    �0 @,0    +   � %    �0!@0  0��0 !        )   �     �0"�!!��@0! � �     c      � ]    �0 �!     `D   �  >    �0!�"    `%   �      �0"@$0  � �     N      � H    �0 �!     `/   �  )    �0!�"    `   @+� �     D      � >    �0 �  0    �0!�"    `   @#0   � �     �      �#  �    �0 � �    �0!� "         `�   �  �    �0"� �    B 0#� $        `�   �
  �    �0$�% `x   �  r    B 0%�& `   0' &&`   0'@�'  @0  �'@0 0'0) 	���     
#/      �# )    �0 �!     `   @+� �     #D      �# >    �0 �!     `%   �      �0!@$0  � �     #�      �+ �    �0 � x    �0!�  j    B 0"�
  Z    �0#�$	 `C   � =    B 0$@0  �@0 ��� �     
+�      � �    �0 � |    �0!�  n    B 0"�
  ^    �0#�$	 `G   � A    B 0$@0  �@�   ��� �     
%      �     �0 @0   � �     r      � l    �0 �  ^    B  0!�
  N    �0"�# `7   �  1    B  0#@"0 0 0%  @+� �     
t      � n    �0 �	 `    �0!�"    `G   �  A    �0"�#    `(   @	0    @ 0   �� �     	%      �     �0 @ 0   � �     F       �  @    �0 � 2    �0!@�    @!0   �� �      =   !   �! 7    �0 �  )    �0!�"    `   @+� �     !F   "   �! @    �0 �  2    �0!�"    `   @0    � �     !�   #   �" �    �0 � }    �0!�	 o    �0"� a    B  0#@�   @	0    @0   � @0    ���� �     	"3   $   �$ -    �0 � !          `   @%� �     $3   %   �$ -    �0 � !          `   @+� �     $=   &   �% 7    �0 �  )    �0!�"    `   @'� �     %   '   �%     �0 @+� �     %�   (   �' �    �0 �  �    �0!@(m   � g    �0"�#
 `P   � J    B 0#@0  0@0  0@&0 ���� �     'S   )   �( M    �0 0!���4   �& #    �0"�!# `   0! ��@)0! � �     (|   *   �) v    �0 �
 h    �0!@
�      @0    2   �  ,    �0"@0 �     0$ ��� �     
)