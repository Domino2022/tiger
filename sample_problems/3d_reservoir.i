[Mesh]
  type = FileMesh
  file = 3d_egs_msh.e
  boundary_id = '1 2 3 4 5 6'
  boundary_name = 'back front left right bottom top'
  block_id = '0 1 2 3 4 5'
  block_name = 'unit_top unit_bottom frac_inclined frac_vertical well_vertical well_inclined'
[]

[MeshModifiers]
  [./inject]
    type = AddExtraNodeset
    new_boundary = inject
    coord = '-300.0 100.0 -1900.0'
    tolerance = 1
  [../]
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
      viscosity = 0.0002
    [../]
  [../]
[]

[UserObjects]
  [./ut_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-15'
  [../]
  [./ub_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-16'
  [../]
  [./fi_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-11'
  [../]
  [./fv_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-10'
  [../]
  [./w_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-5'
  [../]
  [./supg_w]
    type = TigerSUPG
    effective_length = min
    supg_coeficient = transient_tezduyar
  [../]
  [./supg_f]
    type = TigerSUPG
    effective_length = average
    supg_coeficient = transient_tezduyar
  [../]
  [./supg_m]
    type = TigerSUPG
    effective_length = directional_average
    supg_coeficient = transient_tezduyar
  [../]
[]

[GlobalParams]
  pressure = pressure
  conductivity_type = isotropic
  mean_calculation_type = geometric
  has_supg = true
  has_gravity = true
[]

[Materials]
  [./fluid]
    type = TigerFluidMaterial
    fp_uo = water_uo
    temperature = temperature
  [../]
  [./ut_g]
    type = TigerGeometryMaterial
    porosity = 0.1
    block = unit_top
  [../]
  [./ub_g]
    type = TigerGeometryMaterial
    porosity = 0.05
    block = unit_bottom
  [../]
  [./fv_g]
    type = TigerGeometryMaterial
    scale_factor = 0.01
    porosity = 1
    block = frac_vertical
  [../]
  [./fi_g]
    type = TigerGeometryMaterial
    scale_factor = 0.01
    porosity = 1
    block = frac_inclined
  [../]
  [./w_g]
    type = TigerGeometryMaterial
    scale_factor = 0.108
    porosity = 1
    block = 'well_vertical well_inclined'
  [../]
  [./ut_h]
    type = TigerHydraulicMaterialH
    kf_uo = ut_uo
    compressibility = 1.0e-10
    output_properties = 'darcy_velocity'
    outputs = exodus
    block = unit_top
  [../]
  [./ub_h]
    type = TigerHydraulicMaterialH
    kf_uo = ub_uo
    compressibility = 1.0e-10
    output_properties = 'darcy_velocity'
    outputs = exodus
    block = unit_bottom
  [../]
  [./fv_h]
    type = TigerHydraulicMaterialH
    kf_uo = fv_uo
    compressibility = 4.0e-10
    output_properties = 'darcy_velocity'
    outputs = exodus
    block = frac_vertical
  [../]
  [./fi_h]
    type = TigerHydraulicMaterialH
    kf_uo = fi_uo
    compressibility = 4.0e-10
    output_properties = 'darcy_velocity'
    outputs = exodus
    block = frac_inclined
  [../]
  [./w_h]
    type = TigerHydraulicMaterialH
    kf_uo = w_uo
    compressibility = 4.0e-10
    output_properties = 'darcy_velocity'
    outputs = exodus
    block = 'well_vertical well_inclined'
  [../]
  [./ut_t]
    type = TigerThermalMaterialT
    lambda = 2
    density = 2600
    specific_heat = 950
    supg_uo = supg_m
    block = unit_top
  [../]
  [./ub_t]
    type = TigerThermalMaterialT
    lambda = 3
    density = 2600
    specific_heat = 950
    supg_uo = supg_m
    block = unit_bottom
  [../]
  [./f_t]
    type = TigerThermalMaterialT
    lambda = 3
    density = 2600
    specific_heat = 950
    supg_uo = supg_f
    block = 'frac_vertical frac_inclined'
  [../]
  [./w_t]
    type = TigerThermalMaterialT
    lambda = 3
    density = 2600
    specific_heat = 950
    supg_uo = supg_w
    block = 'well_vertical well_inclined'
  [../]
[]

[BCs]
  [./top_h]
    type = DirichletBC
    variable = pressure
    boundary = top
    value = 1.6677e7
  [../]
  [./whole_t]
    type =  DirichletBC
    variable = temperature
    boundary = 'back front left right bottom top'
    value = 473.15
  [../]
  [./well_t]
    type =  DirichletBC
    variable = temperature
    boundary = inject
    value = 343.15
  [../]
[]


[Functions]
  [./hydrostatic]
    type = ParsedFunction
    value = '-1000*9.81*z'
  [../]
  [./temp]
    type = ParsedFunction
    value = 'if(x<-299.9 & x>-300.1 & y<100.1 & y>99.9 & z<-1899.9 & z>-1900.1, 343.15, 473.15)'
  [../]
[]

[ICs]
  [./hydrostatic_ic]
    type = FunctionIC
    variable = pressure
    function = hydrostatic
  [../]
  [./temp_ic]
    type = FunctionIC
    variable = temperature
    function = temp
  [../]
[]

[Variables]
  [./pressure]
    scaling = 1e5
  [../]
  [./temperature]
    scaling = 1e-3
  [../]
[]

[DiracKernels]
  [./pump_in]
    type = TigerHydraulicPointSourceH
    point = '-300.0 100.0 -1900.0'
    mass_flux = -50.0
    variable = pressure
  [../]
  [./pump_out]
    type = TigerHydraulicPointSourceH
    point = '300.0 200.0 -1900.0'
    mass_flux = 50.0
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
  active = 'p1'
  [./p1]
    type = SMP
    full = true
    #petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-pc_type -pc_hypre_type -snes_type -snes_linesearch_type -sub_pc_factor_shift_type'
    petsc_options_value = 'hypre boomeramg newtonls basic NONZERO'
  [../]
  [./p2]
    type = SMP
    full = true
    #petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-pc_type -sub_pc_type -snes_type -snes_linesearch_type -sub_pc_factor_shift_type -ksp_gmres_restart'
    petsc_options_value = 'asm lu newtonls basic NONZERO 51'
  [../]
  [./p3]
    type = SMP
    full = true
    #petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-pc_type -ksp_type -sub_pc_type -snes_type -snes_linesearch_type -pc_asm_overlap -sub_pc_factor_shift_type -ksp_gmres_restart'
    petsc_options_value = 'asm gmres lu newtonls basic 2 NONZERO 51'
  [../]
  [./p4]
    type = FSP
    full = true
    topsplit = pT
    [./pT]
      splitting = 'p T'
      splitting_type = multiplicative
      petsc_options_iname = '-ksp_type -pc_type -snes_type -snes_linesearch_type'
      petsc_options_value = 'fgmres lu newtonls basic'
    [../]
    [./p]
      vars = 'pressure'
      petsc_options_iname = '-ksp_type -pc_type -sub_pc_type'
      petsc_options_value = 'fgmres asm ilu'
    [../]
    [./T]
      vars = 'temperature'
      petsc_options_iname = '-ksp_type -pc_type -pc_hypre_type'
      petsc_options_value = 'preonly hypre boomeramg'
    [../]
  [../]
[]

[Executioner]
  type = Transient
  l_tol = 1e-08
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  l_max_its = 20
  nl_max_its = 20
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 3600
    growth_factor = 10
  [../]
  dtmax = 31536000
  end_time = 946080000
  solve_type = NEWTON
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]
