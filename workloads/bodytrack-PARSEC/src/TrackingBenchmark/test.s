	.file	"test.opt.bc"
	.section	.text.startup,"ax",@progbits
	.align	16, 0x90
	.type	__cxx_global_var_init,@function
__cxx_global_var_init:                  # @__cxx_global_var_init
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movl	$_ZStL8__ioinit, %edi
	callq	_ZNSt8ios_base4InitC1Ev
	movl	$_ZNSt8ios_base4InitD1Ev, %edi
	movl	$_ZStL8__ioinit, %esi
	movl	$__dso_handle, %edx
	callq	__cxa_atexit
	popq	%rax
	ret
.Ltmp2:
	.size	__cxx_global_var_init, .Ltmp2-__cxx_global_var_init
	.cfi_endproc

	.section	.rodata.cst8,"aM",@progbits,8
	.align	8
.LCPI1_0:
	.quad	4602678819172646912     # double 0.5
	.section	.rodata.cst4,"aM",@progbits,4
	.align	4
.LCPI1_1:
	.long	1065353216              # float 1
.LCPI1_2:
	.long	1199440128              # float 65025
	.text
	.globl	_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi
	.align	16, 0x90
	.type	_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi,@function
_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi: # @_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi
	.cfi_startproc
	.cfi_personality 3, __gxx_personality_v0
.Leh_func_begin1:
	.cfi_lsda 3, .Lexception1
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp35:
	.cfi_def_cfa_offset 16
.Ltmp36:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp37:
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	subq	$160, %rsp
.Ltmp38:
	.cfi_offset %rbx, -32
.Ltmp39:
	.cfi_offset %r14, -24
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	%rcx, -48(%rbp)
	movq	%r8, -56(%rbp)
	movq	-24(%rbp), %r14
	leaq	-80(%rbp), %rbx
	movl	$0, -60(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	%rbx, %rdi
	callq	_ZN5PointC1Ev
	movq	-32(%rbp), %rax
	vmovss	8(%rax), %xmm0
	vmovss	12(%rax), %xmm1
	movq	-72(%rbp), %rax
	vsubss	4(%rax), %xmm1, %xmm1
	vsubss	(%rax), %xmm0, %xmm0
.Ltmp3:
	movq	%rbx, %rdi
	callq	_ZN5Point3SetEff
.Ltmp4:
# BB#1:                                 # %invoke.cont
.Ltmp5:
	leaq	-80(%rbp), %rdi
	callq	_Z3magR5Point
.Ltmp6:
# BB#2:                                 # %invoke.cont9
	vmovss	24(%r14), %xmm1
	vcvtss2sd	%xmm1, %xmm0, %xmm1
	vdivsd	%xmm1, %xmm0, %xmm0
	leaq	-100(%rbp), %rdi
	leaq	-104(%rbp), %rsi
	vaddsd	.LCPI1_0(%rip), %xmm0, %xmm0
	vcvttsd2si	%xmm0, %eax
	movl	%eax, -100(%rbp)
	movl	$4, -104(%rbp)
.Ltmp7:
	callq	_ZSt3maxIiERKT_S2_S2_
.Ltmp8:
# BB#3:                                 # %invoke.cont12
	movl	(%rax), %eax
	vcvtsi2ssl	%eax, %xmm0, %xmm0
	vmovss	.LCPI1_1(%rip), %xmm1
	vdivss	%xmm0, %xmm1, %xmm0
	movl	%eax, -96(%rbp)
	incl	%eax
	movl	%eax, -96(%rbp)
	leaq	-128(%rbp), %rdi
	vmovss	%xmm0, -108(%rbp)
	movq	-32(%rbp), %rax
	addq	$16, %rax
	movq	%rax, -120(%rbp)
.Ltmp9:
	callq	_ZN5PointC1Ev
.Ltmp10:
# BB#4:                                 # %invoke.cont18
	movq	-32(%rbp), %rax
	vmovss	24(%rax), %xmm0
	vmovss	28(%rax), %xmm1
	movq	-120(%rbp), %rax
	vsubss	4(%rax), %xmm1, %xmm1
	vsubss	(%rax), %xmm0, %xmm0
.Ltmp11:
	leaq	-128(%rbp), %rdi
	callq	_ZN5Point3SetEff
.Ltmp12:
# BB#5:                                 # %invoke.cont30
.Ltmp13:
	leaq	-128(%rbp), %rdi
	callq	_Z3magR5Point
.Ltmp14:
# BB#6:                                 # %invoke.cont31
	vmovss	24(%r14), %xmm1
	vcvtss2sd	%xmm1, %xmm0, %xmm1
	vdivsd	%xmm1, %xmm0, %xmm0
	leaq	-136(%rbp), %rdi
	leaq	-140(%rbp), %rsi
	vaddsd	.LCPI1_0(%rip), %xmm0, %xmm0
	vcvttsd2si	%xmm0, %eax
	movl	%eax, -136(%rbp)
	movl	$4, -140(%rbp)
.Ltmp15:
	callq	_ZSt3maxIiERKT_S2_S2_
.Ltmp16:
# BB#7:                                 # %invoke.cont40
	movl	(%rax), %eax
	vcvtsi2ssl	%eax, %xmm0, %xmm0
	vmovss	.LCPI1_1(%rip), %xmm1
	vdivss	%xmm0, %xmm1, %xmm0
	movl	%eax, -132(%rbp)
	incl	%eax
	movl	%eax, -132(%rbp)
	vmovss	%xmm0, -144(%rbp)
	movl	$0, -148(%rbp)
	movl	$0, -152(%rbp)
	leaq	-60(%rbp), %rbx
	jmp	.LBB1_8
	.align	16, 0x90
.LBB1_10:                               # %for.inc
                                        #   in Loop: Header=BB1_8 Depth=1
	vmovss	-148(%rbp), %xmm0
	vaddss	-108(%rbp), %xmm0, %xmm0
	vmovss	%xmm0, -148(%rbp)
	incl	-152(%rbp)
.LBB1_8:                                # %for.cond
                                        # =>This Inner Loop Header: Depth=1
	movl	-152(%rbp), %eax
	cmpl	-96(%rbp), %eax
	jge	.LBB1_15
# BB#9:                                 # %for.body
                                        #   in Loop: Header=BB1_8 Depth=1
	vmovss	-148(%rbp), %xmm0
	vmulss	-80(%rbp), %xmm0, %xmm0
	movq	-72(%rbp), %rax
	vaddss	(%rax), %xmm0, %xmm0
	vmovss	%xmm0, -156(%rbp)
	vmovss	-148(%rbp), %xmm0
	vmulss	-76(%rbp), %xmm0, %xmm0
	movq	-72(%rbp), %rax
	vaddss	4(%rax), %xmm0, %xmm1
	vmovss	%xmm1, -160(%rbp)
	movq	-56(%rbp), %rdx
	movq	-40(%rbp), %rdi
	vmovss	-156(%rbp), %xmm0
.Ltmp24:
	movq	%rbx, %rsi
	callq	_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_
.Ltmp25:
	jmp	.LBB1_10
.LBB1_12:                               # %lpad29.nonloopexit.loopexit
.Ltmp26:
	jmp	.LBB1_14
.LBB1_15:                               # %for.end
	movl	$0, -148(%rbp)
	movl	$0, -164(%rbp)
	leaq	-60(%rbp), %rbx
	jmp	.LBB1_16
	.align	16, 0x90
.LBB1_18:                               # %for.inc73
                                        #   in Loop: Header=BB1_16 Depth=1
	vmovss	-148(%rbp), %xmm0
	vaddss	-144(%rbp), %xmm0, %xmm0
	vmovss	%xmm0, -148(%rbp)
	incl	-164(%rbp)
.LBB1_16:                               # %for.cond58
                                        # =>This Inner Loop Header: Depth=1
	movl	-164(%rbp), %eax
	cmpl	-132(%rbp), %eax
	jge	.LBB1_19
# BB#17:                                # %for.body60
                                        #   in Loop: Header=BB1_16 Depth=1
	vmovss	-148(%rbp), %xmm0
	vmulss	-128(%rbp), %xmm0, %xmm0
	movq	-120(%rbp), %rax
	vaddss	(%rax), %xmm0, %xmm0
	vmovss	%xmm0, -168(%rbp)
	vmovss	-148(%rbp), %xmm0
	vmulss	-124(%rbp), %xmm0, %xmm0
	movq	-120(%rbp), %rax
	vaddss	4(%rax), %xmm0, %xmm1
	vmovss	%xmm1, -172(%rbp)
	movq	-56(%rbp), %rdx
	movq	-40(%rbp), %rdi
	vmovss	-168(%rbp), %xmm0
.Ltmp21:
	movq	%rbx, %rsi
	callq	_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_
.Ltmp22:
	jmp	.LBB1_18
.LBB1_11:                               # %lpad29.loopexit
.Ltmp23:
	jmp	.LBB1_14
.LBB1_19:                               # %for.end75
	vcvtsi2ssl	-60(%rbp), %xmm0, %xmm0
	vdivss	.LCPI1_2(%rip), %xmm0, %xmm0
	leaq	-128(%rbp), %rdi
	movq	-48(%rbp), %rax
	vaddss	(%rax), %xmm0, %xmm0
	vmovss	%xmm0, (%rax)
.Ltmp18:
	callq	_ZN5PointD1Ev
.Ltmp19:
# BB#20:                                # %invoke.cont79
	leaq	-80(%rbp), %rdi
	callq	_ZN5PointD1Ev
	addq	$160, %rsp
	popq	%rbx
	popq	%r14
	popq	%rbp
	ret
.LBB1_13:                               # %lpad29.nonloopexit.nonloopexit
.Ltmp17:
.LBB1_14:                               # %lpad29
	movq	%rax, -88(%rbp)
	movl	%edx, -92(%rbp)
.Ltmp27:
	leaq	-128(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp28:
	jmp	.LBB1_22
.LBB1_21:                               # %lpad
.Ltmp20:
	movq	%rax, -88(%rbp)
	movl	%edx, -92(%rbp)
.LBB1_22:                               # %ehcleanup
.Ltmp29:
	leaq	-80(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp30:
# BB#23:                                # %eh.resume
	movq	-88(%rbp), %rdi
	callq	_Unwind_Resume
.LBB1_24:                               # %terminate.lpad
.Ltmp31:
	movq	%rax, %rdi
	callq	__clang_call_terminate
.Ltmp40:
	.size	_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi, .Ltmp40-_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi
	.cfi_endproc
.Leh_func_end1:
	.section	.gcc_except_table,"a",@progbits
	.align	4
GCC_except_table1:
.Lexception1:
	.byte	255                     # @LPStart Encoding = omit
	.byte	3                       # @TType Encoding = udata4
	.byte	125                     # @TType base offset
	.byte	3                       # Call site Encoding = udata4
	.byte	117                     # Call site table length
.Lset0 = .Leh_func_begin1-.Leh_func_begin1 # >> Call Site 1 <<
	.long	.Lset0
.Lset1 = .Ltmp3-.Leh_func_begin1        #   Call between .Leh_func_begin1 and .Ltmp3
	.long	.Lset1
	.long	0                       #     has no landing pad
	.byte	0                       #   On action: cleanup
.Lset2 = .Ltmp3-.Leh_func_begin1        # >> Call Site 2 <<
	.long	.Lset2
.Lset3 = .Ltmp10-.Ltmp3                 #   Call between .Ltmp3 and .Ltmp10
	.long	.Lset3
.Lset4 = .Ltmp20-.Leh_func_begin1       #     jumps to .Ltmp20
	.long	.Lset4
	.byte	0                       #   On action: cleanup
.Lset5 = .Ltmp11-.Leh_func_begin1       # >> Call Site 3 <<
	.long	.Lset5
.Lset6 = .Ltmp16-.Ltmp11                #   Call between .Ltmp11 and .Ltmp16
	.long	.Lset6
.Lset7 = .Ltmp17-.Leh_func_begin1       #     jumps to .Ltmp17
	.long	.Lset7
	.byte	0                       #   On action: cleanup
.Lset8 = .Ltmp24-.Leh_func_begin1       # >> Call Site 4 <<
	.long	.Lset8
.Lset9 = .Ltmp25-.Ltmp24                #   Call between .Ltmp24 and .Ltmp25
	.long	.Lset9
.Lset10 = .Ltmp26-.Leh_func_begin1      #     jumps to .Ltmp26
	.long	.Lset10
	.byte	0                       #   On action: cleanup
.Lset11 = .Ltmp21-.Leh_func_begin1      # >> Call Site 5 <<
	.long	.Lset11
.Lset12 = .Ltmp22-.Ltmp21               #   Call between .Ltmp21 and .Ltmp22
	.long	.Lset12
.Lset13 = .Ltmp23-.Leh_func_begin1      #     jumps to .Ltmp23
	.long	.Lset13
	.byte	0                       #   On action: cleanup
.Lset14 = .Ltmp18-.Leh_func_begin1      # >> Call Site 6 <<
	.long	.Lset14
.Lset15 = .Ltmp19-.Ltmp18               #   Call between .Ltmp18 and .Ltmp19
	.long	.Lset15
.Lset16 = .Ltmp20-.Leh_func_begin1      #     jumps to .Ltmp20
	.long	.Lset16
	.byte	0                       #   On action: cleanup
.Lset17 = .Ltmp19-.Leh_func_begin1      # >> Call Site 7 <<
	.long	.Lset17
.Lset18 = .Ltmp27-.Ltmp19               #   Call between .Ltmp19 and .Ltmp27
	.long	.Lset18
	.long	0                       #     has no landing pad
	.byte	0                       #   On action: cleanup
.Lset19 = .Ltmp27-.Leh_func_begin1      # >> Call Site 8 <<
	.long	.Lset19
.Lset20 = .Ltmp30-.Ltmp27               #   Call between .Ltmp27 and .Ltmp30
	.long	.Lset20
.Lset21 = .Ltmp31-.Leh_func_begin1      #     jumps to .Ltmp31
	.long	.Lset21
	.byte	1                       #   On action: 1
.Lset22 = .Ltmp30-.Leh_func_begin1      # >> Call Site 9 <<
	.long	.Lset22
.Lset23 = .Leh_func_end1-.Ltmp30        #   Call between .Ltmp30 and .Leh_func_end1
	.long	.Lset23
	.long	0                       #     has no landing pad
	.byte	0                       #   On action: cleanup
	.byte	1                       # >> Action Record 1 <<
                                        #   Catch TypeInfo 1
	.byte	0                       #   No further actions
                                        # >> Catch TypeInfos <<
	.long	0                       # TypeInfo 1
	.align	4

	.section	.text._ZN5PointC1Ev,"axG",@progbits,_ZN5PointC1Ev,comdat
	.weak	_ZN5PointC1Ev
	.align	16, 0x90
	.type	_ZN5PointC1Ev,@function
_ZN5PointC1Ev:                          # @_ZN5PointC1Ev
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp43:
	.cfi_def_cfa_offset 16
.Ltmp44:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp45:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	callq	_ZN5PointC2Ev
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp46:
	.size	_ZN5PointC1Ev, .Ltmp46-_ZN5PointC1Ev
	.cfi_endproc

	.section	.text._ZN5Point3SetEff,"axG",@progbits,_ZN5Point3SetEff,comdat
	.weak	_ZN5Point3SetEff
	.align	16, 0x90
	.type	_ZN5Point3SetEff,@function
_ZN5Point3SetEff:                       # @_ZN5Point3SetEff
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp49:
	.cfi_def_cfa_offset 16
.Ltmp50:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp51:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	vmovss	%xmm0, -12(%rbp)
	vmovss	%xmm1, -16(%rbp)
	vmovss	-12(%rbp), %xmm0
	movq	-8(%rbp), %rax
	vmovss	%xmm0, (%rax)
	vmovss	-16(%rbp), %xmm0
	vmovss	%xmm0, 4(%rax)
	popq	%rbp
	ret
.Ltmp52:
	.size	_ZN5Point3SetEff, .Ltmp52-_ZN5Point3SetEff
	.cfi_endproc

	.section	.text._ZSt3maxIiERKT_S2_S2_,"axG",@progbits,_ZSt3maxIiERKT_S2_S2_,comdat
	.weak	_ZSt3maxIiERKT_S2_S2_
	.align	16, 0x90
	.type	_ZSt3maxIiERKT_S2_S2_,@function
_ZSt3maxIiERKT_S2_S2_:                  # @_ZSt3maxIiERKT_S2_S2_
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp55:
	.cfi_def_cfa_offset 16
.Ltmp56:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp57:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	cmpl	(%rsi), %eax
	jge	.LBB4_2
# BB#1:                                 # %if.then
	movq	-24(%rbp), %rax
	jmp	.LBB4_3
.LBB4_2:                                # %if.end
	movq	-16(%rbp), %rax
.LBB4_3:                                # %if.end
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	ret
.Ltmp58:
	.size	_ZSt3maxIiERKT_S2_S2_, .Ltmp58-_ZSt3maxIiERKT_S2_S2_
	.cfi_endproc

	.section	.text._Z3magR5Point,"axG",@progbits,_Z3magR5Point,comdat
	.weak	_Z3magR5Point
	.align	16, 0x90
	.type	_Z3magR5Point,@function
_Z3magR5Point:                          # @_Z3magR5Point
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp61:
	.cfi_def_cfa_offset 16
.Ltmp62:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp63:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	vmovss	(%rdi), %xmm0
	vmovss	4(%rdi), %xmm1
	vmulss	%xmm1, %xmm1, %xmm1
	vmulss	%xmm0, %xmm0, %xmm0
	vaddss	%xmm1, %xmm0, %xmm0
	vcvtss2sd	%xmm0, %xmm0, %xmm0
	callq	sqrt
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp64:
	.size	_Z3magR5Point, .Ltmp64-_Z3magR5Point
	.cfi_endproc

	.section	.rodata.cst4,"aM",@progbits,4
	.align	4
.LCPI6_0:
	.long	1056964608              # float 0.5
	.section	.text._Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_,"axG",@progbits,_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_,comdat
	.weak	_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_
	.align	16, 0x90
	.type	_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_,@function
_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_: # @_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp68:
	.cfi_def_cfa_offset 16
.Ltmp69:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp70:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	subq	$56, %rsp
.Ltmp71:
	.cfi_offset %rbx, -24
	vmovss	%xmm0, -12(%rbp)
	vmovss	%xmm1, -16(%rbp)
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	vmovss	.LCPI6_0(%rip), %xmm0
	vaddss	-12(%rbp), %xmm0, %xmm1
	vcvttss2si	%xmm1, %eax
	movl	%eax, -44(%rbp)
	vaddss	-16(%rbp), %xmm0, %xmm0
	vcvttss2si	%xmm0, %eax
	movl	%eax, -48(%rbp)
	cmpl	$0, -44(%rbp)
	js	.LBB6_5
# BB#1:                                 # %land.lhs.true
	movl	-44(%rbp), %ebx
	movq	-24(%rbp), %rdi
	callq	_ZNK9FlexImageIhLi1EE5WidthEv
	cmpl	%eax, %ebx
	jge	.LBB6_5
# BB#2:                                 # %land.lhs.true4
	cmpl	$0, -48(%rbp)
	js	.LBB6_5
# BB#3:                                 # %land.lhs.true6
	movl	-48(%rbp), %ebx
	movq	-24(%rbp), %rdi
	callq	_ZNK9FlexImageIhLi1EE6HeightEv
	cmpl	%eax, %ebx
	jge	.LBB6_5
# BB#4:                                 # %if.then
	movl	-48(%rbp), %edx
	movl	-44(%rbp), %esi
	movq	-24(%rbp), %rdi
	callq	_ZNK9FlexImageIhLi1EEclEii
	movzbl	(%rax), %ecx
	movl	$255, %eax
	subl	%ecx, %eax
	movl	%eax, -52(%rbp)
	imull	%eax, %eax
	movq	-32(%rbp), %rcx
	addl	%eax, (%rcx)
	movq	-40(%rbp), %rax
	incl	(%rax)
.LBB6_5:                                # %if.end
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	ret
.Ltmp72:
	.size	_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_, .Ltmp72-_Z15SampleEdgePointffRK9FlexImageIhLi1EERiS3_
	.cfi_endproc

	.section	.text._ZN5PointD1Ev,"axG",@progbits,_ZN5PointD1Ev,comdat
	.weak	_ZN5PointD1Ev
	.align	16, 0x90
	.type	_ZN5PointD1Ev,@function
_ZN5PointD1Ev:                          # @_ZN5PointD1Ev
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp75:
	.cfi_def_cfa_offset 16
.Ltmp76:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp77:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	callq	_ZN5PointD2Ev
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp78:
	.size	_ZN5PointD1Ev, .Ltmp78-_ZN5PointD1Ev
	.cfi_endproc

	.section	.text.__clang_call_terminate,"axG",@progbits,__clang_call_terminate,comdat
	.hidden	__clang_call_terminate
	.weak	__clang_call_terminate
	.align	16, 0x90
	.type	__clang_call_terminate,@function
__clang_call_terminate:                 # @__clang_call_terminate
# BB#0:
	pushq	%rbp
	movq	%rsp, %rbp
	callq	__cxa_begin_catch
	callq	_ZSt9terminatev
.Ltmp79:
	.size	__clang_call_terminate, .Ltmp79-__clang_call_terminate

	.section	.rodata.cst4,"aM",@progbits,4
	.align	4
.LCPI9_0:
	.long	1073741824              # float 2
.LCPI9_2:
	.long	1065353216              # float 1
	.section	.rodata.cst8,"aM",@progbits,8
	.align	8
.LCPI9_1:
	.quad	4602678819172646912     # double 0.5
	.text
	.globl	_ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_
	.align	16, 0x90
	.type	_ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_,@function
_ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_: # @_ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_
	.cfi_startproc
	.cfi_personality 3, __gxx_personality_v0
.Leh_func_begin9:
	.cfi_lsda 3, .Lexception9
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp136:
	.cfi_def_cfa_offset 16
.Ltmp137:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp138:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%rbx
	subq	$168, %rsp
.Ltmp139:
	.cfi_offset %rbx, -40
.Ltmp140:
	.cfi_offset %r14, -32
.Ltmp141:
	.cfi_offset %r15, -24
	movq	%rdi, -32(%rbp)
	movq	%rsi, -40(%rbp)
	movq	%rdx, -48(%rbp)
	movq	%rcx, -56(%rbp)
	movq	%r8, -64(%rbp)
	movq	-32(%rbp), %rbx
	leaq	-88(%rbp), %rdi
	movq	-40(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	-40(%rbp), %rax
	addq	$24, %rax
	movq	%rax, -80(%rbp)
	callq	_ZN5PointC1Ev
	leaq	-96(%rbp), %rdi
.Ltmp80:
	callq	_ZN5PointC1Ev
.Ltmp81:
# BB#1:                                 # %invoke.cont
	movq	-40(%rbp), %rax
	vmovss	8(%rax), %xmm0
	vmovss	12(%rax), %xmm1
	movq	-72(%rbp), %rax
	vsubss	4(%rax), %xmm1, %xmm1
	vsubss	(%rax), %xmm0, %xmm0
.Ltmp82:
	leaq	-88(%rbp), %rdi
	callq	_ZN5Point3SetEff
.Ltmp83:
# BB#2:                                 # %invoke.cont12
	movq	-40(%rbp), %rax
	vmovss	16(%rax), %xmm0
	vmovss	20(%rax), %xmm1
	movq	-80(%rbp), %rax
	vsubss	4(%rax), %xmm1, %xmm1
	vsubss	(%rax), %xmm0, %xmm0
.Ltmp84:
	leaq	-96(%rbp), %rdi
	callq	_ZN5Point3SetEff
.Ltmp85:
# BB#3:                                 # %invoke.cont23
	vmovss	-96(%rbp), %xmm0
	vmovss	-92(%rbp), %xmm2
	vmovss	.LCPI9_0(%rip), %xmm3
	vmovss	-88(%rbp), %xmm4
	vmovss	-84(%rbp), %xmm1
	vdivss	%xmm3, %xmm1, %xmm1
	vdivss	%xmm3, %xmm2, %xmm2
	vdivss	%xmm3, %xmm4, %xmm4
	vdivss	%xmm3, %xmm0, %xmm0
	movq	-72(%rbp), %rax
	movq	-80(%rbp), %rcx
	leaq	-120(%rbp), %rdi
	vaddss	(%rcx), %xmm0, %xmm0
	vaddss	(%rax), %xmm4, %xmm3
	vsubss	%xmm0, %xmm3, %xmm0
	vaddss	4(%rcx), %xmm2, %xmm2
	vaddss	4(%rax), %xmm1, %xmm1
	vsubss	%xmm2, %xmm1, %xmm1
.Ltmp86:
	callq	_ZN5PointC1Eff
.Ltmp87:
# BB#4:                                 # %invoke.cont40
.Ltmp88:
	leaq	-88(%rbp), %rdi
	callq	_Z3magR5Point
.Ltmp89:
# BB#5:                                 # %invoke.cont42
	vmovss	32(%rbx), %xmm1
	vcvtss2sd	%xmm1, %xmm0, %xmm1
	vdivsd	%xmm1, %xmm0, %xmm0
	leaq	-128(%rbp), %rdi
	leaq	-132(%rbp), %rsi
	vaddsd	.LCPI9_1(%rip), %xmm0, %xmm0
	vcvttsd2si	%xmm0, %eax
	movl	%eax, -128(%rbp)
	movl	$4, -132(%rbp)
.Ltmp90:
	callq	_ZSt3maxIiERKT_S2_S2_
.Ltmp91:
# BB#6:                                 # %invoke.cont47
	movl	(%rax), %eax
	movl	%eax, -124(%rbp)
.Ltmp92:
	leaq	-120(%rbp), %rdi
	callq	_Z3magR5Point
.Ltmp93:
# BB#7:                                 # %invoke.cont49
	vmovss	28(%rbx), %xmm1
	vcvtss2sd	%xmm1, %xmm0, %xmm1
	vdivsd	%xmm1, %xmm0, %xmm0
	leaq	-140(%rbp), %rdi
	leaq	-144(%rbp), %rsi
	vaddsd	.LCPI9_1(%rip), %xmm0, %xmm0
	vcvttsd2si	%xmm0, %eax
	movl	%eax, -140(%rbp)
	movl	$4, -144(%rbp)
.Ltmp94:
	callq	_ZSt3maxIiERKT_S2_S2_
.Ltmp95:
# BB#8:                                 # %invoke.cont57
	movl	(%rax), %eax
	movl	%eax, -136(%rbp)
	movl	-124(%rbp), %eax
	vcvtsi2ssl	%eax, %xmm0, %xmm1
	vmovss	.LCPI9_2(%rip), %xmm0
	vdivss	%xmm1, %xmm0, %xmm1
	incl	%eax
	movl	%eax, -124(%rbp)
	leaq	-168(%rbp), %rdi
	vmovss	%xmm1, -148(%rbp)
	vcvtsi2ssl	-136(%rbp), %xmm0, %xmm1
	vdivss	%xmm1, %xmm0, %xmm0
	vmovss	%xmm0, -152(%rbp)
	movl	$0, -156(%rbp)
.Ltmp96:
	callq	_ZN5PointC1Ev
.Ltmp97:
# BB#9:                                 # %invoke.cont63
.Ltmp98:
	leaq	-176(%rbp), %rdi
	callq	_ZN5PointC1Ev
.Ltmp99:
# BB#10:                                # %invoke.cont65
	movl	$0, -180(%rbp)
	leaq	-168(%rbp), %r14
	leaq	-176(%rbp), %r15
	leaq	-120(%rbp), %rbx
	jmp	.LBB9_11
	.align	16, 0x90
.LBB9_25:                               # %for.inc106
                                        #   in Loop: Header=BB9_11 Depth=1
	incl	-180(%rbp)
.LBB9_11:                               # %for.cond
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB9_16 Depth 2
	movl	-180(%rbp), %eax
	cmpl	-124(%rbp), %eax
	jge	.LBB9_26
# BB#12:                                # %for.body
                                        #   in Loop: Header=BB9_11 Depth=1
	vmovss	-156(%rbp), %xmm0
	vmulss	-84(%rbp), %xmm0, %xmm1
	movq	-72(%rbp), %rax
	vaddss	4(%rax), %xmm1, %xmm1
	vmulss	-88(%rbp), %xmm0, %xmm0
	vaddss	(%rax), %xmm0, %xmm0
.Ltmp112:
	movq	%r14, %rdi
	callq	_ZN5Point3SetEff
.Ltmp113:
# BB#13:                                # %invoke.cont74
                                        #   in Loop: Header=BB9_11 Depth=1
	vmovss	-156(%rbp), %xmm0
	vmulss	-92(%rbp), %xmm0, %xmm1
	movq	-80(%rbp), %rax
	vaddss	4(%rax), %xmm1, %xmm1
	vmulss	-96(%rbp), %xmm0, %xmm0
	vaddss	(%rax), %xmm0, %xmm0
.Ltmp114:
	movq	%r15, %rdi
	callq	_ZN5Point3SetEff
.Ltmp115:
# BB#14:                                # %invoke.cont83
                                        #   in Loop: Header=BB9_11 Depth=1
	vmovss	-176(%rbp), %xmm0
	vmovss	-172(%rbp), %xmm1
	vsubss	-164(%rbp), %xmm1, %xmm1
	vsubss	-168(%rbp), %xmm0, %xmm0
.Ltmp116:
	movq	%rbx, %rdi
	callq	_ZN5Point3SetEff
.Ltmp117:
# BB#15:                                # %invoke.cont90
                                        #   in Loop: Header=BB9_11 Depth=1
	vmovss	-156(%rbp), %xmm0
	vaddss	-148(%rbp), %xmm0, %xmm0
	vmovss	%xmm0, -156(%rbp)
	movl	$0, -184(%rbp)
	movl	$0, -188(%rbp)
	jmp	.LBB9_16
	.align	16, 0x90
.LBB9_18:                               # %for.inc
                                        #   in Loop: Header=BB9_16 Depth=2
	vmovss	-184(%rbp), %xmm0
	vaddss	-152(%rbp), %xmm0, %xmm0
	vmovss	%xmm0, -184(%rbp)
	incl	-188(%rbp)
.LBB9_16:                               # %for.cond92
                                        #   Parent Loop BB9_11 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-188(%rbp), %eax
	cmpl	-136(%rbp), %eax
	jge	.LBB9_25
# BB#17:                                # %for.body94
                                        #   in Loop: Header=BB9_16 Depth=2
	vmovss	-184(%rbp), %xmm1
	vmulss	-120(%rbp), %xmm1, %xmm0
	vmulss	-116(%rbp), %xmm1, %xmm1
	vaddss	-164(%rbp), %xmm1, %xmm1
	vaddss	-168(%rbp), %xmm0, %xmm0
	movq	-64(%rbp), %rdx
	movq	-56(%rbp), %rsi
	movq	-48(%rbp), %rdi
.Ltmp119:
	callq	_Z17SampleInsidePointffRK11BinaryImageRiS2_
.Ltmp120:
	jmp	.LBB9_18
.LBB9_22:                               # %lpad73.loopexit
.Ltmp121:
	jmp	.LBB9_24
.LBB9_26:                               # %for.end108
.Ltmp100:
	leaq	-176(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp101:
# BB#27:                                # %invoke.cont109
.Ltmp103:
	leaq	-168(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp104:
# BB#28:                                # %invoke.cont111
.Ltmp106:
	leaq	-120(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp107:
# BB#29:                                # %invoke.cont113
.Ltmp109:
	leaq	-96(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp110:
# BB#30:                                # %invoke.cont116
	leaq	-88(%rbp), %rdi
	callq	_ZN5PointD1Ev
	addq	$168, %rsp
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
.LBB9_23:                               # %lpad73.nonloopexit
.Ltmp118:
.LBB9_24:                               # %lpad73
	movq	%rax, -104(%rbp)
	movl	%edx, -108(%rbp)
.Ltmp122:
	leaq	-176(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp123:
	jmp	.LBB9_32
.LBB9_31:                               # %lpad64
.Ltmp102:
	movq	%rax, -104(%rbp)
	movl	%edx, -108(%rbp)
.LBB9_32:                               # %ehcleanup
.Ltmp124:
	leaq	-168(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp125:
	jmp	.LBB9_33
.LBB9_21:                               # %lpad41
.Ltmp105:
	movq	%rax, -104(%rbp)
	movl	%edx, -108(%rbp)
.LBB9_33:                               # %ehcleanup114
.Ltmp126:
	leaq	-120(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp127:
	jmp	.LBB9_34
.LBB9_20:                               # %lpad11
.Ltmp108:
	movq	%rax, -104(%rbp)
	movl	%edx, -108(%rbp)
.LBB9_34:                               # %ehcleanup117
.Ltmp128:
	leaq	-96(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp129:
	jmp	.LBB9_35
.LBB9_19:                               # %lpad
.Ltmp111:
	movq	%rax, -104(%rbp)
	movl	%edx, -108(%rbp)
.LBB9_35:                               # %ehcleanup119
.Ltmp130:
	leaq	-88(%rbp), %rdi
	callq	_ZN5PointD1Ev
.Ltmp131:
# BB#36:                                # %eh.resume
	movq	-104(%rbp), %rdi
	callq	_Unwind_Resume
.LBB9_37:                               # %terminate.lpad
.Ltmp132:
	movq	%rax, %rdi
	callq	__clang_call_terminate
.Ltmp142:
	.size	_ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_, .Ltmp142-_ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_
	.cfi_endproc
.Leh_func_end9:
	.section	.gcc_except_table,"a",@progbits
	.align	4
GCC_except_table9:
.Lexception9:
	.byte	255                     # @LPStart Encoding = omit
	.byte	3                       # @TType Encoding = udata4
	.asciz	 "\277\201"             # @TType base offset
	.byte	3                       # Call site Encoding = udata4
	.ascii	 "\266\001"             # Call site table length
.Lset24 = .Leh_func_begin9-.Leh_func_begin9 # >> Call Site 1 <<
	.long	.Lset24
.Lset25 = .Ltmp80-.Leh_func_begin9      #   Call between .Leh_func_begin9 and .Ltmp80
	.long	.Lset25
	.long	0                       #     has no landing pad
	.byte	0                       #   On action: cleanup
.Lset26 = .Ltmp80-.Leh_func_begin9      # >> Call Site 2 <<
	.long	.Lset26
.Lset27 = .Ltmp81-.Ltmp80               #   Call between .Ltmp80 and .Ltmp81
	.long	.Lset27
.Lset28 = .Ltmp111-.Leh_func_begin9     #     jumps to .Ltmp111
	.long	.Lset28
	.byte	0                       #   On action: cleanup
.Lset29 = .Ltmp82-.Leh_func_begin9      # >> Call Site 3 <<
	.long	.Lset29
.Lset30 = .Ltmp87-.Ltmp82               #   Call between .Ltmp82 and .Ltmp87
	.long	.Lset30
.Lset31 = .Ltmp108-.Leh_func_begin9     #     jumps to .Ltmp108
	.long	.Lset31
	.byte	0                       #   On action: cleanup
.Lset32 = .Ltmp88-.Leh_func_begin9      # >> Call Site 4 <<
	.long	.Lset32
.Lset33 = .Ltmp97-.Ltmp88               #   Call between .Ltmp88 and .Ltmp97
	.long	.Lset33
.Lset34 = .Ltmp105-.Leh_func_begin9     #     jumps to .Ltmp105
	.long	.Lset34
	.byte	0                       #   On action: cleanup
.Lset35 = .Ltmp98-.Leh_func_begin9      # >> Call Site 5 <<
	.long	.Lset35
.Lset36 = .Ltmp99-.Ltmp98               #   Call between .Ltmp98 and .Ltmp99
	.long	.Lset36
.Lset37 = .Ltmp102-.Leh_func_begin9     #     jumps to .Ltmp102
	.long	.Lset37
	.byte	0                       #   On action: cleanup
.Lset38 = .Ltmp112-.Leh_func_begin9     # >> Call Site 6 <<
	.long	.Lset38
.Lset39 = .Ltmp117-.Ltmp112             #   Call between .Ltmp112 and .Ltmp117
	.long	.Lset39
.Lset40 = .Ltmp118-.Leh_func_begin9     #     jumps to .Ltmp118
	.long	.Lset40
	.byte	0                       #   On action: cleanup
.Lset41 = .Ltmp119-.Leh_func_begin9     # >> Call Site 7 <<
	.long	.Lset41
.Lset42 = .Ltmp120-.Ltmp119             #   Call between .Ltmp119 and .Ltmp120
	.long	.Lset42
.Lset43 = .Ltmp121-.Leh_func_begin9     #     jumps to .Ltmp121
	.long	.Lset43
	.byte	0                       #   On action: cleanup
.Lset44 = .Ltmp100-.Leh_func_begin9     # >> Call Site 8 <<
	.long	.Lset44
.Lset45 = .Ltmp101-.Ltmp100             #   Call between .Ltmp100 and .Ltmp101
	.long	.Lset45
.Lset46 = .Ltmp102-.Leh_func_begin9     #     jumps to .Ltmp102
	.long	.Lset46
	.byte	0                       #   On action: cleanup
.Lset47 = .Ltmp103-.Leh_func_begin9     # >> Call Site 9 <<
	.long	.Lset47
.Lset48 = .Ltmp104-.Ltmp103             #   Call between .Ltmp103 and .Ltmp104
	.long	.Lset48
.Lset49 = .Ltmp105-.Leh_func_begin9     #     jumps to .Ltmp105
	.long	.Lset49
	.byte	0                       #   On action: cleanup
.Lset50 = .Ltmp106-.Leh_func_begin9     # >> Call Site 10 <<
	.long	.Lset50
.Lset51 = .Ltmp107-.Ltmp106             #   Call between .Ltmp106 and .Ltmp107
	.long	.Lset51
.Lset52 = .Ltmp108-.Leh_func_begin9     #     jumps to .Ltmp108
	.long	.Lset52
	.byte	0                       #   On action: cleanup
.Lset53 = .Ltmp109-.Leh_func_begin9     # >> Call Site 11 <<
	.long	.Lset53
.Lset54 = .Ltmp110-.Ltmp109             #   Call between .Ltmp109 and .Ltmp110
	.long	.Lset54
.Lset55 = .Ltmp111-.Leh_func_begin9     #     jumps to .Ltmp111
	.long	.Lset55
	.byte	0                       #   On action: cleanup
.Lset56 = .Ltmp110-.Leh_func_begin9     # >> Call Site 12 <<
	.long	.Lset56
.Lset57 = .Ltmp122-.Ltmp110             #   Call between .Ltmp110 and .Ltmp122
	.long	.Lset57
	.long	0                       #     has no landing pad
	.byte	0                       #   On action: cleanup
.Lset58 = .Ltmp122-.Leh_func_begin9     # >> Call Site 13 <<
	.long	.Lset58
.Lset59 = .Ltmp131-.Ltmp122             #   Call between .Ltmp122 and .Ltmp131
	.long	.Lset59
.Lset60 = .Ltmp132-.Leh_func_begin9     #     jumps to .Ltmp132
	.long	.Lset60
	.byte	1                       #   On action: 1
.Lset61 = .Ltmp131-.Leh_func_begin9     # >> Call Site 14 <<
	.long	.Lset61
.Lset62 = .Leh_func_end9-.Ltmp131       #   Call between .Ltmp131 and .Leh_func_end9
	.long	.Lset62
	.long	0                       #     has no landing pad
	.byte	0                       #   On action: cleanup
	.byte	1                       # >> Action Record 1 <<
                                        #   Catch TypeInfo 1
	.byte	0                       #   No further actions
                                        # >> Catch TypeInfos <<
	.long	0                       # TypeInfo 1
	.align	4

	.section	.text._ZN5PointC1Eff,"axG",@progbits,_ZN5PointC1Eff,comdat
	.weak	_ZN5PointC1Eff
	.align	16, 0x90
	.type	_ZN5PointC1Eff,@function
_ZN5PointC1Eff:                         # @_ZN5PointC1Eff
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp145:
	.cfi_def_cfa_offset 16
.Ltmp146:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp147:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	vmovss	%xmm0, -12(%rbp)
	vmovss	%xmm1, -16(%rbp)
	vmovss	-12(%rbp), %xmm0
	movq	-8(%rbp), %rdi
	callq	_ZN5PointC2Eff
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp148:
	.size	_ZN5PointC1Eff, .Ltmp148-_ZN5PointC1Eff
	.cfi_endproc

	.section	.rodata.cst4,"aM",@progbits,4
	.align	4
.LCPI11_0:
	.long	1056964608              # float 0.5
	.section	.text._Z17SampleInsidePointffRK11BinaryImageRiS2_,"axG",@progbits,_Z17SampleInsidePointffRK11BinaryImageRiS2_,comdat
	.weak	_Z17SampleInsidePointffRK11BinaryImageRiS2_
	.align	16, 0x90
	.type	_Z17SampleInsidePointffRK11BinaryImageRiS2_,@function
_Z17SampleInsidePointffRK11BinaryImageRiS2_: # @_Z17SampleInsidePointffRK11BinaryImageRiS2_
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp152:
	.cfi_def_cfa_offset 16
.Ltmp153:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp154:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	subq	$56, %rsp
.Ltmp155:
	.cfi_offset %rbx, -24
	vmovss	%xmm0, -12(%rbp)
	vmovss	%xmm1, -16(%rbp)
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	vmovss	.LCPI11_0(%rip), %xmm0
	vaddss	-12(%rbp), %xmm0, %xmm1
	vcvttss2si	%xmm1, %eax
	movl	%eax, -44(%rbp)
	vaddss	-16(%rbp), %xmm0, %xmm0
	vcvttss2si	%xmm0, %eax
	movl	%eax, -48(%rbp)
	cmpl	$0, -44(%rbp)
	js	.LBB11_5
# BB#1:                                 # %land.lhs.true
	movl	-44(%rbp), %ebx
	movq	-24(%rbp), %rdi
	callq	_ZNK11BinaryImage5WidthEv
	cmpl	%eax, %ebx
	jge	.LBB11_5
# BB#2:                                 # %land.lhs.true4
	cmpl	$0, -48(%rbp)
	js	.LBB11_5
# BB#3:                                 # %land.lhs.true6
	movl	-48(%rbp), %ebx
	movq	-24(%rbp), %rdi
	callq	_ZNK11BinaryImage6HeightEv
	cmpl	%eax, %ebx
	jge	.LBB11_5
# BB#4:                                 # %if.then
	movl	-48(%rbp), %edx
	movl	-44(%rbp), %esi
	movq	-24(%rbp), %rdi
	callq	_ZNK11BinaryImageclEii
	movl	$1, %ecx
	subl	%eax, %ecx
	movl	%ecx, -52(%rbp)
	movq	-32(%rbp), %rax
	addl	%ecx, (%rax)
	movq	-40(%rbp), %rax
	incl	(%rax)
.LBB11_5:                               # %if.end
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	ret
.Ltmp156:
	.size	_Z17SampleInsidePointffRK11BinaryImageRiS2_, .Ltmp156-_Z17SampleInsidePointffRK11BinaryImageRiS2_
	.cfi_endproc

	.text
	.globl	_ZN17ImageMeasurements14ImageErrorEdgeERSt6vectorI9FlexImageIhLi1EESaIS2_EER24MultiCameraProjectedBody
	.align	16, 0x90
	.type	_ZN17ImageMeasurements14ImageErrorEdgeERSt6vectorI9FlexImageIhLi1EESaIS2_EER24MultiCameraProjectedBody,@function
_ZN17ImageMeasurements14ImageErrorEdgeERSt6vectorI9FlexImageIhLi1EESaIS2_EER24MultiCameraProjectedBody: # @_ZN17ImageMeasurements14ImageErrorEdgeERSt6vectorI9FlexImageIhLi1EESaIS2_EER24MultiCameraProjectedBody
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp160:
	.cfi_def_cfa_offset 16
.Ltmp161:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp162:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
	subq	$48, %rsp
.Ltmp163:
	.cfi_offset %rbx, -48
.Ltmp164:
	.cfi_offset %r12, -40
.Ltmp165:
	.cfi_offset %r14, -32
.Ltmp166:
	.cfi_offset %r15, -24
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-40(%rbp), %r12
	movl	$0, -60(%rbp)
	movl	$0, -64(%rbp)
	movl	$0, -68(%rbp)
	leaq	-64(%rbp), %r14
	leaq	-60(%rbp), %r15
	jmp	.LBB12_1
	.align	16, 0x90
.LBB12_5:                               # %for.inc11
                                        #   in Loop: Header=BB12_1 Depth=1
	incl	-68(%rbp)
.LBB12_1:                               # %for.cond
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB12_3 Depth 2
	movl	-68(%rbp), %ebx
	movq	-48(%rbp), %rdi
	callq	_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv
	cmpl	%eax, %ebx
	jge	.LBB12_6
# BB#2:                                 # %for.body
                                        #   in Loop: Header=BB12_1 Depth=1
	movl	-68(%rbp), %esi
	movq	-56(%rbp), %rdi
	callq	_ZN24MultiCameraProjectedBodyclEi
	movq	%rax, %rdi
	callq	_ZN13ProjectedBody4SizeEv
	movl	%eax, -72(%rbp)
	movl	$0, -76(%rbp)
	jmp	.LBB12_3
	.align	16, 0x90
.LBB12_4:                               # %for.inc
                                        #   in Loop: Header=BB12_3 Depth=2
	movl	-68(%rbp), %esi
	movq	-56(%rbp), %rdi
	callq	_ZN24MultiCameraProjectedBodyclEi
	movl	-76(%rbp), %esi
	movq	%rax, %rdi
	callq	_ZN13ProjectedBodyclEi
	movq	%rax, %rbx
	movslq	-68(%rbp), %rsi
	movq	-48(%rbp), %rdi
	callq	_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm
	movq	%r12, %rdi
	movq	%rbx, %rsi
	movq	%rax, %rdx
	movq	%r14, %rcx
	movq	%r15, %r8
	callq	_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi
	incl	-76(%rbp)
.LBB12_3:                               # %for.cond4
                                        #   Parent Loop BB12_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-76(%rbp), %eax
	cmpl	-72(%rbp), %eax
	jge	.LBB12_5
	jmp	.LBB12_4
.LBB12_6:                               # %for.end13
	vcvtsi2ssl	-60(%rbp), %xmm0, %xmm0
	vmovss	-64(%rbp), %xmm1
	vdivss	%xmm0, %xmm1, %xmm0
	addq	$48, %rsp
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
.Ltmp167:
	.size	_ZN17ImageMeasurements14ImageErrorEdgeERSt6vectorI9FlexImageIhLi1EESaIS2_EER24MultiCameraProjectedBody, .Ltmp167-_ZN17ImageMeasurements14ImageErrorEdgeERSt6vectorI9FlexImageIhLi1EESaIS2_EER24MultiCameraProjectedBody
	.cfi_endproc

	.section	.text._ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv,"axG",@progbits,_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv,comdat
	.weak	_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv
	.align	16, 0x90
	.type	_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv,@function
_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv: # @_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp170:
	.cfi_def_cfa_offset 16
.Ltmp171:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp172:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	8(%rdi), %rcx
	subq	(%rdi), %rcx
	sarq	$3, %rcx
	movabsq	$-3689348814741910323, %rax # imm = 0xCCCCCCCCCCCCCCCD
	imulq	%rcx, %rax
	popq	%rbp
	ret
.Ltmp173:
	.size	_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv, .Ltmp173-_ZNKSt6vectorI9FlexImageIhLi1EESaIS1_EE4sizeEv
	.cfi_endproc

	.section	.text._ZN24MultiCameraProjectedBodyclEi,"axG",@progbits,_ZN24MultiCameraProjectedBodyclEi,comdat
	.weak	_ZN24MultiCameraProjectedBodyclEi
	.align	16, 0x90
	.type	_ZN24MultiCameraProjectedBodyclEi,@function
_ZN24MultiCameraProjectedBodyclEi:      # @_ZN24MultiCameraProjectedBodyclEi
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp176:
	.cfi_def_cfa_offset 16
.Ltmp177:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp178:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movslq	-12(%rbp), %rsi
	movq	-8(%rbp), %rdi
	callq	_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp179:
	.size	_ZN24MultiCameraProjectedBodyclEi, .Ltmp179-_ZN24MultiCameraProjectedBodyclEi
	.cfi_endproc

	.section	.text._ZN13ProjectedBody4SizeEv,"axG",@progbits,_ZN13ProjectedBody4SizeEv,comdat
	.weak	_ZN13ProjectedBody4SizeEv
	.align	16, 0x90
	.type	_ZN13ProjectedBody4SizeEv,@function
_ZN13ProjectedBody4SizeEv:              # @_ZN13ProjectedBody4SizeEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp182:
	.cfi_def_cfa_offset 16
.Ltmp183:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp184:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	callq	_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv
                                        # kill: EAX<def> EAX<kill> RAX<kill>
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp185:
	.size	_ZN13ProjectedBody4SizeEv, .Ltmp185-_ZN13ProjectedBody4SizeEv
	.cfi_endproc

	.section	.text._ZN13ProjectedBodyclEi,"axG",@progbits,_ZN13ProjectedBodyclEi,comdat
	.weak	_ZN13ProjectedBodyclEi
	.align	16, 0x90
	.type	_ZN13ProjectedBodyclEi,@function
_ZN13ProjectedBodyclEi:                 # @_ZN13ProjectedBodyclEi
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp188:
	.cfi_def_cfa_offset 16
.Ltmp189:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp190:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movslq	-12(%rbp), %rsi
	movq	-8(%rbp), %rdi
	callq	_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp191:
	.size	_ZN13ProjectedBodyclEi, .Ltmp191-_ZN13ProjectedBodyclEi
	.cfi_endproc

	.section	.text._ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm,"axG",@progbits,_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm,comdat
	.weak	_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm
	.align	16, 0x90
	.type	_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm,@function
_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm: # @_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp194:
	.cfi_def_cfa_offset 16
.Ltmp195:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp196:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	leaq	(%rsi,%rsi,4), %rax
	shlq	$3, %rax
	movq	-8(%rbp), %rcx
	addq	(%rcx), %rax
	popq	%rbp
	ret
.Ltmp197:
	.size	_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm, .Ltmp197-_ZNSt6vectorI9FlexImageIhLi1EESaIS1_EEixEm
	.cfi_endproc

	.text
	.globl	_ZN17ImageMeasurements16ImageErrorInsideERSt6vectorI11BinaryImageSaIS1_EER24MultiCameraProjectedBody
	.align	16, 0x90
	.type	_ZN17ImageMeasurements16ImageErrorInsideERSt6vectorI11BinaryImageSaIS1_EER24MultiCameraProjectedBody,@function
_ZN17ImageMeasurements16ImageErrorInsideERSt6vectorI11BinaryImageSaIS1_EER24MultiCameraProjectedBody: # @_ZN17ImageMeasurements16ImageErrorInsideERSt6vectorI11BinaryImageSaIS1_EER24MultiCameraProjectedBody
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp201:
	.cfi_def_cfa_offset 16
.Ltmp202:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp203:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
	subq	$48, %rsp
.Ltmp204:
	.cfi_offset %rbx, -48
.Ltmp205:
	.cfi_offset %r12, -40
.Ltmp206:
	.cfi_offset %r14, -32
.Ltmp207:
	.cfi_offset %r15, -24
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-40(%rbp), %r12
	movl	$0, -60(%rbp)
	movl	$0, -64(%rbp)
	movl	$0, -68(%rbp)
	leaq	-64(%rbp), %r14
	leaq	-60(%rbp), %r15
	jmp	.LBB18_1
	.align	16, 0x90
.LBB18_5:                               # %for.inc11
                                        #   in Loop: Header=BB18_1 Depth=1
	incl	-68(%rbp)
.LBB18_1:                               # %for.cond
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB18_3 Depth 2
	movl	-68(%rbp), %ebx
	movq	-48(%rbp), %rdi
	callq	_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv
	cmpl	%eax, %ebx
	jge	.LBB18_6
# BB#2:                                 # %for.body
                                        #   in Loop: Header=BB18_1 Depth=1
	movl	-68(%rbp), %esi
	movq	-56(%rbp), %rdi
	callq	_ZN24MultiCameraProjectedBodyclEi
	movq	%rax, %rdi
	callq	_ZN13ProjectedBody4SizeEv
	movl	%eax, -72(%rbp)
	movl	$0, -76(%rbp)
	jmp	.LBB18_3
	.align	16, 0x90
.LBB18_4:                               # %for.inc
                                        #   in Loop: Header=BB18_3 Depth=2
	movl	-68(%rbp), %esi
	movq	-56(%rbp), %rdi
	callq	_ZN24MultiCameraProjectedBodyclEi
	movl	-76(%rbp), %esi
	movq	%rax, %rdi
	callq	_ZN13ProjectedBodyclEi
	movq	%rax, %rbx
	movslq	-68(%rbp), %rsi
	movq	-48(%rbp), %rdi
	callq	_ZNSt6vectorI11BinaryImageSaIS0_EEixEm
	movq	%r12, %rdi
	movq	%rbx, %rsi
	movq	%rax, %rdx
	movq	%r14, %rcx
	movq	%r15, %r8
	callq	_ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_
	incl	-76(%rbp)
.LBB18_3:                               # %for.cond4
                                        #   Parent Loop BB18_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-76(%rbp), %eax
	cmpl	-72(%rbp), %eax
	jge	.LBB18_5
	jmp	.LBB18_4
.LBB18_6:                               # %for.end13
	vcvtsi2ssl	-60(%rbp), %xmm0, %xmm0
	vcvtsi2ssl	-64(%rbp), %xmm0, %xmm1
	vdivss	%xmm0, %xmm1, %xmm0
	addq	$48, %rsp
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
.Ltmp208:
	.size	_ZN17ImageMeasurements16ImageErrorInsideERSt6vectorI11BinaryImageSaIS1_EER24MultiCameraProjectedBody, .Ltmp208-_ZN17ImageMeasurements16ImageErrorInsideERSt6vectorI11BinaryImageSaIS1_EER24MultiCameraProjectedBody
	.cfi_endproc

	.section	.text._ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv,"axG",@progbits,_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv,comdat
	.weak	_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv
	.align	16, 0x90
	.type	_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv,@function
_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv: # @_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp211:
	.cfi_def_cfa_offset 16
.Ltmp212:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp213:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	8(%rdi), %rcx
	subq	(%rdi), %rcx
	sarq	$3, %rcx
	movabsq	$-6148914691236517205, %rax # imm = 0xAAAAAAAAAAAAAAAB
	imulq	%rcx, %rax
	popq	%rbp
	ret
.Ltmp214:
	.size	_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv, .Ltmp214-_ZNKSt6vectorI11BinaryImageSaIS0_EE4sizeEv
	.cfi_endproc

	.section	.text._ZNSt6vectorI11BinaryImageSaIS0_EEixEm,"axG",@progbits,_ZNSt6vectorI11BinaryImageSaIS0_EEixEm,comdat
	.weak	_ZNSt6vectorI11BinaryImageSaIS0_EEixEm
	.align	16, 0x90
	.type	_ZNSt6vectorI11BinaryImageSaIS0_EEixEm,@function
_ZNSt6vectorI11BinaryImageSaIS0_EEixEm: # @_ZNSt6vectorI11BinaryImageSaIS0_EEixEm
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp217:
	.cfi_def_cfa_offset 16
.Ltmp218:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp219:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	leaq	(%rsi,%rsi,2), %rax
	shlq	$3, %rax
	movq	-8(%rbp), %rcx
	addq	(%rcx), %rax
	popq	%rbp
	ret
.Ltmp220:
	.size	_ZNSt6vectorI11BinaryImageSaIS0_EEixEm, .Ltmp220-_ZNSt6vectorI11BinaryImageSaIS0_EEixEm
	.cfi_endproc

	.section	.text._ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm,"axG",@progbits,_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm,comdat
	.weak	_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm
	.align	16, 0x90
	.type	_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm,@function
_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm: # @_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp223:
	.cfi_def_cfa_offset 16
.Ltmp224:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp225:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	shlq	$5, %rsi
	movq	-8(%rbp), %rax
	addq	(%rax), %rsi
	movq	%rsi, %rax
	popq	%rbp
	ret
.Ltmp226:
	.size	_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm, .Ltmp226-_ZNSt6vectorI17ProjectedCylinderSaIS0_EEixEm
	.cfi_endproc

	.section	.text._ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv,"axG",@progbits,_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv,comdat
	.weak	_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv
	.align	16, 0x90
	.type	_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv,@function
_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv: # @_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp229:
	.cfi_def_cfa_offset 16
.Ltmp230:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp231:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	8(%rdi), %rax
	subq	(%rdi), %rax
	sarq	$5, %rax
	popq	%rbp
	ret
.Ltmp232:
	.size	_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv, .Ltmp232-_ZNKSt6vectorI17ProjectedCylinderSaIS0_EE4sizeEv
	.cfi_endproc

	.section	.text._ZNSt6vectorI13ProjectedBodySaIS0_EEixEm,"axG",@progbits,_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm,comdat
	.weak	_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm
	.align	16, 0x90
	.type	_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm,@function
_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm: # @_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp235:
	.cfi_def_cfa_offset 16
.Ltmp236:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp237:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	leaq	(%rsi,%rsi,2), %rax
	shlq	$3, %rax
	movq	-8(%rbp), %rcx
	addq	(%rcx), %rax
	popq	%rbp
	ret
.Ltmp238:
	.size	_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm, .Ltmp238-_ZNSt6vectorI13ProjectedBodySaIS0_EEixEm
	.cfi_endproc

	.section	.text._ZNK11BinaryImage5WidthEv,"axG",@progbits,_ZNK11BinaryImage5WidthEv,comdat
	.weak	_ZNK11BinaryImage5WidthEv
	.align	16, 0x90
	.type	_ZNK11BinaryImage5WidthEv,@function
_ZNK11BinaryImage5WidthEv:              # @_ZNK11BinaryImage5WidthEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp241:
	.cfi_def_cfa_offset 16
.Ltmp242:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp243:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	16(%rdi), %eax
	popq	%rbp
	ret
.Ltmp244:
	.size	_ZNK11BinaryImage5WidthEv, .Ltmp244-_ZNK11BinaryImage5WidthEv
	.cfi_endproc

	.section	.text._ZNK11BinaryImage6HeightEv,"axG",@progbits,_ZNK11BinaryImage6HeightEv,comdat
	.weak	_ZNK11BinaryImage6HeightEv
	.align	16, 0x90
	.type	_ZNK11BinaryImage6HeightEv,@function
_ZNK11BinaryImage6HeightEv:             # @_ZNK11BinaryImage6HeightEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp247:
	.cfi_def_cfa_offset 16
.Ltmp248:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp249:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	20(%rdi), %eax
	popq	%rbp
	ret
.Ltmp250:
	.size	_ZNK11BinaryImage6HeightEv, .Ltmp250-_ZNK11BinaryImage6HeightEv
	.cfi_endproc

	.section	.text._ZNK11BinaryImageclEii,"axG",@progbits,_ZNK11BinaryImageclEii,comdat
	.weak	_ZNK11BinaryImageclEii
	.align	16, 0x90
	.type	_ZNK11BinaryImageclEii,@function
_ZNK11BinaryImageclEii:                 # @_ZNK11BinaryImageclEii
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp253:
	.cfi_def_cfa_offset 16
.Ltmp254:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp255:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	imull	16(%rax), %edx
	addl	-12(%rbp), %edx
	movl	%edx, -20(%rbp)
	movl	%edx, %ecx
	sarl	$31, %ecx
	shrl	$29, %ecx
	addl	%edx, %ecx
	sarl	$3, %ecx
	movl	%ecx, -24(%rbp)
	movq	8(%rax), %rax
	movslq	-24(%rbp), %rcx
	movzbl	(%rax,%rcx), %eax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	sarl	$31, %edx
	shrl	$29, %edx
	addl	%ecx, %edx
	andl	$248, %edx
	subl	%edx, %ecx
                                        # kill: CL<def> CL<kill> ECX<kill>
	shrl	%cl, %eax
	andl	$1, %eax
	popq	%rbp
	ret
.Ltmp256:
	.size	_ZNK11BinaryImageclEii, .Ltmp256-_ZNK11BinaryImageclEii
	.cfi_endproc

	.section	.text._ZN5PointC2Eff,"axG",@progbits,_ZN5PointC2Eff,comdat
	.weak	_ZN5PointC2Eff
	.align	16, 0x90
	.type	_ZN5PointC2Eff,@function
_ZN5PointC2Eff:                         # @_ZN5PointC2Eff
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp259:
	.cfi_def_cfa_offset 16
.Ltmp260:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp261:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	vmovss	%xmm0, -12(%rbp)
	vmovss	%xmm1, -16(%rbp)
	vmovss	-12(%rbp), %xmm0
	movq	-8(%rbp), %rax
	vmovss	%xmm0, (%rax)
	vmovss	-16(%rbp), %xmm0
	vmovss	%xmm0, 4(%rax)
	popq	%rbp
	ret
.Ltmp262:
	.size	_ZN5PointC2Eff, .Ltmp262-_ZN5PointC2Eff
	.cfi_endproc

	.section	.text._ZN5PointD2Ev,"axG",@progbits,_ZN5PointD2Ev,comdat
	.weak	_ZN5PointD2Ev
	.align	16, 0x90
	.type	_ZN5PointD2Ev,@function
_ZN5PointD2Ev:                          # @_ZN5PointD2Ev
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp265:
	.cfi_def_cfa_offset 16
.Ltmp266:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp267:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	popq	%rbp
	ret
.Ltmp268:
	.size	_ZN5PointD2Ev, .Ltmp268-_ZN5PointD2Ev
	.cfi_endproc

	.section	.text._ZNK9FlexImageIhLi1EE5WidthEv,"axG",@progbits,_ZNK9FlexImageIhLi1EE5WidthEv,comdat
	.weak	_ZNK9FlexImageIhLi1EE5WidthEv
	.align	16, 0x90
	.type	_ZNK9FlexImageIhLi1EE5WidthEv,@function
_ZNK9FlexImageIhLi1EE5WidthEv:          # @_ZNK9FlexImageIhLi1EE5WidthEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp271:
	.cfi_def_cfa_offset 16
.Ltmp272:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp273:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	24(%rdi), %eax
	popq	%rbp
	ret
.Ltmp274:
	.size	_ZNK9FlexImageIhLi1EE5WidthEv, .Ltmp274-_ZNK9FlexImageIhLi1EE5WidthEv
	.cfi_endproc

	.section	.text._ZNK9FlexImageIhLi1EE6HeightEv,"axG",@progbits,_ZNK9FlexImageIhLi1EE6HeightEv,comdat
	.weak	_ZNK9FlexImageIhLi1EE6HeightEv
	.align	16, 0x90
	.type	_ZNK9FlexImageIhLi1EE6HeightEv,@function
_ZNK9FlexImageIhLi1EE6HeightEv:         # @_ZNK9FlexImageIhLi1EE6HeightEv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp277:
	.cfi_def_cfa_offset 16
.Ltmp278:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp279:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	28(%rdi), %eax
	popq	%rbp
	ret
.Ltmp280:
	.size	_ZNK9FlexImageIhLi1EE6HeightEv, .Ltmp280-_ZNK9FlexImageIhLi1EE6HeightEv
	.cfi_endproc

	.section	.text._ZNK9FlexImageIhLi1EEclEii,"axG",@progbits,_ZNK9FlexImageIhLi1EEclEii,comdat
	.weak	_ZNK9FlexImageIhLi1EEclEii
	.align	16, 0x90
	.type	_ZNK9FlexImageIhLi1EEclEii,@function
_ZNK9FlexImageIhLi1EEclEii:             # @_ZNK9FlexImageIhLi1EEclEii
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp283:
	.cfi_def_cfa_offset 16
.Ltmp284:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp285:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rdi
	xorl	%ecx, %ecx
	callq	_ZNK9FlexImageIhLi1EEclEiii
	addq	$16, %rsp
	popq	%rbp
	ret
.Ltmp286:
	.size	_ZNK9FlexImageIhLi1EEclEii, .Ltmp286-_ZNK9FlexImageIhLi1EEclEii
	.cfi_endproc

	.section	.text._ZNK9FlexImageIhLi1EEclEiii,"axG",@progbits,_ZNK9FlexImageIhLi1EEclEiii,comdat
	.weak	_ZNK9FlexImageIhLi1EEclEiii
	.align	16, 0x90
	.type	_ZNK9FlexImageIhLi1EEclEiii,@function
_ZNK9FlexImageIhLi1EEclEiii:            # @_ZNK9FlexImageIhLi1EEclEiii
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp289:
	.cfi_def_cfa_offset 16
.Ltmp290:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp291:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	%ecx, -20(%rbp)
	movq	-8(%rbp), %rcx
	movl	16(%rcx), %eax
	movl	20(%rcx), %edx
	imull	-16(%rbp), %edx
	movslq	%edx, %rdx
	addq	8(%rcx), %rdx
	imull	-12(%rbp), %eax
	movslq	%eax, %rcx
	addq	%rdx, %rcx
	movslq	-20(%rbp), %rax
	addq	%rcx, %rax
	popq	%rbp
	ret
.Ltmp292:
	.size	_ZNK9FlexImageIhLi1EEclEiii, .Ltmp292-_ZNK9FlexImageIhLi1EEclEiii
	.cfi_endproc

	.section	.text._ZN5PointC2Ev,"axG",@progbits,_ZN5PointC2Ev,comdat
	.weak	_ZN5PointC2Ev
	.align	16, 0x90
	.type	_ZN5PointC2Ev,@function
_ZN5PointC2Ev:                          # @_ZN5PointC2Ev
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp295:
	.cfi_def_cfa_offset 16
.Ltmp296:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp297:
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	popq	%rbp
	ret
.Ltmp298:
	.size	_ZN5PointC2Ev, .Ltmp298-_ZN5PointC2Ev
	.cfi_endproc

	.section	.text.startup,"ax",@progbits
	.align	16, 0x90
	.type	_GLOBAL__I_a,@function
_GLOBAL__I_a:                           # @_GLOBAL__I_a
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp301:
	.cfi_def_cfa_offset 16
.Ltmp302:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp303:
	.cfi_def_cfa_register %rbp
	callq	__cxx_global_var_init
	popq	%rbp
	ret
.Ltmp304:
	.size	_GLOBAL__I_a, .Ltmp304-_GLOBAL__I_a
	.cfi_endproc

	.type	_ZStL8__ioinit,@object  # @_ZStL8__ioinit
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.ctors,"aw",@progbits
	.align	8
	.quad	_GLOBAL__I_a
	.type	__axc_function_list_string__,@object # @__axc_function_list_string__
	.section	.data.__axc_function_list_string__,"aGw",@progbits,__axc_function_list_string__,comdat
	.weak	__axc_function_list_string__
	.align	16
__axc_function_list_string__:
	.asciz	 "_ZN17ImageMeasurements9EdgeErrorERK17ProjectedCylinderRK9FlexImageIhLi1EERfRi _ZN17ImageMeasurements11InsideErrorERK17ProjectedCylinderRK11BinaryImageRiS6_ _ZN17ImageMeasurements14ImageErrorEdgeERSt6vectorI9FlexImageIhLi1EESaIS2_EER24MultiCameraProjectedBody _ZN17ImageMeasurements16ImageErrorInsideERSt6vectorI11BinaryImageSaIS1_EER24MultiCameraProjectedBody "
	.size	__axc_function_list_string__, 361

	.type	__axc_precision_reduce_function_list_string__,@object # @__axc_precision_reduce_function_list_string__
	.section	.bss.__axc_precision_reduce_function_list_string__,"aGw",@nobits,__axc_precision_reduce_function_list_string__,comdat
	.weak	__axc_precision_reduce_function_list_string__
__axc_precision_reduce_function_list_string__:
	.zero	1
	.size	__axc_precision_reduce_function_list_string__, 1


	.section	".note.GNU-stack","",@progbits
