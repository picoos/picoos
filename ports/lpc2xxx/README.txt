- Toolchain is same as cortex-m

- OpenOCD for my lpc2129 required:
  lpc2xxx.cfg: reset_config trst_and_srst srst_pulls_trst
  lpc2129.cfg: setup_lpc2xxx lpc2129 0x4f1f0f0f <-- hex value is different
  lpc2129:cfg: setup_lpc2129 14745 <-- crystal is different
