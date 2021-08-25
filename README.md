# Overview
This is a series of execises meant to teach you how to write Verilog as well as
some hardware design techniques that are often used. These are roughly drawn
from skills or concepts that I have found particularly important when building
hardware (and others have as well, going by discussions I've had).

# Overall Goal
These exercises do assume some preexisting knowledge of basic hardware concepts
and are not meant to replace a basic digital design class. Rather, they are
meant to supplement online resources on digital design as practical exercises.
Additionally, they focus on teaching synthesizable SystemVerilog with good
design. That means that the Verilog can be run through both ASIC and FPGA tools
with a high probability of inferring correctly.

This means that we will not cover a lot of nonsynthesizable Verilog constructs
and if you have previously learned Verilog through a class or online resource,
it can feel overly pedantic. However, Verilog is full of 
[hidden](https://lcdm-eng.com/papers/snug06_Verilog%20Gotchas%20Part1.pdf)
[surprises](https://sutherland-hdl.com/papers/2007-SNUG-SanJose_gotcha_again_paper.pdf),
that can cause your design to infer incorrectly or different tools can infer
different designs from the same Verilog.

The best way to avoid these sorts of problems writing Verilog that follows
certain patterns. The exercises are designed to insist on you using these
patterns as much as possible and the exercise write-up will call out important
patterns

## Exercise 1
In this exercise, you build a 4-input multiplexer from 2-input multiplexers. The
point of this is to make sure you have a working Verilator environment as well
as introduce a subset of Verilog syntax.

## Exercise 2
In this exercise, you build a state machine for a counter. The point of this
exercise is to introduce the rest of the Verilog syntax needed for these
exercises as well as introducing state machines as a concept and sequential
versus combinational logic. It also emphasizes good style practices for
implementing state machines as well as sequential versus combinational logic.

## Exercise 3
This exercise consists of building two designs. The first is a multiported RAM
and the second is a multiplier. The point of both is to learn how to use and
implement `val\rdy` handshakes as well as understand some of the timing
intracies involved.

## Exercise 4
This exercise is the most involved and consists of implementing both the control
and datapaths of a processor pipeline that implements a minimal set of (5)
RISC-V instructions. The point of this exericse is to introduce pipelining as a
concept as well as some good style practices around implementing pipelines.