[Mesh]
  type = FileMesh
  file = ex_hdr2.msh
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
      density = 1000
      viscosity = 0.0002
      cp = 4200
      thermal_conductivity = 0.65
      bulk_modulus = 2.5e+09
    [../]
  [../]
[]

[UserObjects]
  [./matrix_uo1]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-15'
  [../]
  [./fracture_uo1]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '8.333333e-10'
  [../]
  [./supg] # for outputing Pe_Cr_numbers needed
    type = TigerSUPG
    effective_length = min
    supg_coeficient = optimal
  [../]
[]

[GlobalParams]
  pressure = pressure
  conductivity_type = isotropic
  mean_calculation_type = geometric
  output_Pe_Cr_numbers = true
  supg_uo = supg # for outputing Pe_Cr_numbers needed
[]

[Materials]
  [./fluid]
    type = TigerFluidMaterial
    fp_uo = water_uo
    temperature = temperature
  [../]
  [./matrix_g]
    type = TigerGeometryMaterial
    porosity = 0.1
    scale_factor = 100
    block = 'unit'
  [../]
  [./matrix_h]
    type = TigerHydraulicMaterialH
    kf_uo = matrix_uo1
    compressibility = 1.0e-10
    output_properties = 'darcy_velocity'
    outputs = exodus
    block = 'unit'
  [../]
  [./matrix_t]
    type = TigerThermalMaterialT
    lambda = 3
    density = 2600
    specific_heat = 950
    output_properties = 'thermal_peclet_number thermal_courant_number'
    outputs = exodus
    block = 'unit'
  [../]
  [./fracture_g]
    type = TigerGeometryMaterial
    porosity = 1
    scale_factor = 0.01
    block = 'frac1 frac2'
  [../]
  [./fracure_h]
    type = TigerHydraulicMaterialH
    kf_uo = fracture_uo1
    compressibility = 4.0e-10
    output_properties = 'darcy_velocity'
    outputs = exodus
    block = 'frac1 frac2'
  [../]
  [./fracture_t]
    type = TigerThermalMaterialT
    lambda = 3
    density = 2600
    specific_heat = 950
    output_properties = 'thermal_peclet_number thermal_courant_number'
    outputs = exodus
    block = 'frac1 frac2'
  [../]
[]

[BCs]
  [./whole_h]
    type = DirichletBC
    variable = pressure
    boundary = circum
    value = 1e7
  [../]
  [./whole_t]
    type =  DirichletBC
    variable = temperature
    boundary = circum
    value = 423.15
  [../]
  [./well_t]
    type =  DirichletBC
    variable = temperature
    boundary = inject
    value = 343.15
  [../]
[]

[Variables]
  [./pressure]
    initial_condition = 1e7
    scaling = 1e8
  [../]
  [./temperature]
    initial_condition = 423.15
  [../]
[]

[DiracKernels]
  [./pump_in]
    type = TigerHydraulicPointSourceH
    point = '150.0 250.0 0.0'
    mass_flux = -3.0
    variable = pressure
  [../]
  [./pump_out]
    type = TigerHydraulicPointSourceH
    point = '350.0 250.0 0.0'
    mass_flux = 3.0
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
  [./T_advect]
    type = TigerThermalAdvectionKernelT
    variable = temperature
  [../]
  [./T_diff]
    type = TigerThermalDiffusionKernelT
    variable = temperature
  [../]
  [./T_time]
    type = TigerThermalTimeKernelT
    variable = temperature
  [../]
[]

[Preconditioning]
  active = 'p2'
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
    dt = 3600
    growth_factor = 2
  [../]
  dtmax = 31536000
  end_time = 1892160000
  solve_type = NEWTON
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]
