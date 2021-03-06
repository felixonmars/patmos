/*
 * Copyright: 2017, Technical University of Denmark, DTU Compute
 * Author: Martin Schoeberl (martin@jopdesign.com)
 * License: Simplified BSD License
 */

package twoway

import Chisel._

class Port(size: Int) extends Bundle {
  val addr = UInt(width = log2Up(size)).asInput
  val wrData = UInt(width = 32).asInput
  val rdData = UInt(width = 32).asOutput
  val wrEna = Bool().asInput
}

class DualPort(size: Int) extends Bundle {
  val rdAddr = UInt(width = log2Up(size)).asInput
  val wrAddr = UInt(width = log2Up(size)).asInput
  val wrData = UInt(width = 32).asInput
  val rdData = UInt(width = 32).asOutput
  val wrEna = Bool().asInput
}

class DualPortMemory(size: Int) extends Module {
  val io = new Bundle {
    val port = new DualPort(size)
  }

  val mem = Mem(UInt(width = 32), size)

  io.port.rdData := mem(Reg(next = io.port.rdAddr, init = UInt(0)))
  when(io.port.wrEna) {
    mem(io.port.wrAddr) := io.port.wrData
  }

}

/**
 * This true dual port memory generates registers.
 */
class TrueDualPortMemory(size: Int) extends Module {
  val io = new Bundle {
    val portA = new Port(size)
    val portB = new Port(size)
  }

  val mem = Mem(UInt(width = 32), size)

  val regAddrA = Reg(io.portA.addr, init = UInt(0))
  when(io.portA.wrEna) {
    mem(io.portA.addr) := io.portA.wrData
  }.otherwise {
    regAddrA := io.portA.addr
  }
  io.portA.rdData := mem(regAddrA)

  // This does not generate a true dual-ported memory,
  // but a register based implementation
  val regAddrB = Reg(io.portB.addr, init = UInt(0))
  when(io.portB.wrEna) {
    mem(io.portB.addr) := io.portB.wrData
  }.otherwise {
    regAddrB := io.portB.addr
  }
  io.portB.rdData := mem(regAddrB)
}
