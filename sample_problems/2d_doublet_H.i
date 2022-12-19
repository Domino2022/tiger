[Mesh]
  type = GeneratedMesh
  dim = 2
  xmax = 500
  ymax = 500
  nx = 100
  ny = 100
[]

[MeshModifiers]
  [./center]
    type = AddExtraNodeset
    new_boundary = center
    coord = '0 0'
  [../]
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
      density = 1000
      viscosity = 0.001
      bulk_modulus = 1e+10
    [../]
  [../]
[]

[UserObjects]
  [./matrix_uo1]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-14'
  [../]
[]

[Materials]
  [./fluid]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./matrix_g]
    type = TigerGeometryMaterial
    porosity = 0
    scale_factor = 200
  [../]
  [./matrix_h]
    type = TigerHydraulicMaterialH
    pressure = pressure
    kf_uo = matrix_uo1
    compressibility = 1.0e-10
  [../]
[]

[BCs]
  [./center_h]
    type = DirichletBC
    variable = pressure
    boundary = center
    value = 1e7
  [../]
[]

[Variables]
  [./pressure]
    initial_condition = 1e7
    # scaling = 1e8
  [../]
[]

[DiracKernels]
  [./pump_in]
    type = TigerHydraulicPointSourceH
    point = '175.0 250.0 0.0'
    mass_flux = -1.0
    variable = pressure
  [../]
  [./pump_out]
    type = TigerHydraulicPointSourceH
    point = '325.0 250.0 0.0'
    mass_flux = 1.0
    variable = pressure
  [../]
[]

[Kernels]
  [./H_diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
  [./H_time]
    type = TigerHydraulicTimeKernelH
    variable = pressure
  [../]
[]

[Preconditioning]
  active = 'p1'
  [./p1]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_hypre_type -ksp_rtol -ksp_atol -ksp_max_it -snes_rtol -snes_atol'
    petsc_options_value = 'hypre boomeramg 1e-12 1e-10 20 1e-8 1e-10'
  [../]
  [./p2]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -sub_pc_type -ksp_rtol -ksp_atol -ksp_max_it -snes_rtol -snes_atol -sub_pc_factor_shift_type'
    petsc_options_value = 'asm lu 1e-12 1e-10 20 1e-8 1e-10 NONZERO'
  [../]
  [./p3]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -ksp_type -sub_pc_type -pc_asm_overlap -ksp_rtol -ksp_atol -ksp_max_it -snes_rtol -snes_atol -sub_pc_factor_shift_type'
    petsc_options_value = 'asm gmres lu 2 1e-12 1e-10 20 1e-8 1e-10 NONZERO'
  [../]
[]

[Executioner]
  type = Transient
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1
    growth_factor = 2
  [../]
  dtmax = 3600
  end_time = 864000
  solve_type = NEWTON
  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]
