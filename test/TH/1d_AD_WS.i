[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = 0
  xmax = 1
  nx = 10
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
      density = 1
      cp = 1
      thermal_conductivity = 0.01
    [../]
  [../]
[]

[UserObjects]
  [./matrix_uo1]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-8'
  [../]
  [./supg]
    type = TigerSUPG
    effective_length = min
    supg_coeficient = optimal
  [../]
[]

[Materials]
  [./rock_g]
    type = TigerGeometryMaterial
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 1
    specific_density = 1
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./matrix_h]
    type = TigerHydraulicMaterialH
    pressure = pressure
    kf_uo = matrix_uo1
    compressibility = 1.0e-10
  [../]
  [./matrix_t]
    type = TigerThermalMaterialT
    conductivity_type = isotropic
    mean_calculation_type = arithmetic
    lambda = 0.01
    specific_heat = 1
    has_supg = true
    supg_uo = supg
  [../]
[]

[BCs]
  [./left_h]
    type = DirichletBC
    variable = pressure
    boundary = left
    value = 1e5
  [../]
  [./right_h]
    type = DirichletBC
    variable = pressure
    boundary = right
    value = 0
  [../]
  [./left_t]
    type = DirichletBC
    variable = temperature
    boundary = left
    value = 0
  [../]
  [./right_t]
    type = DirichletBC
    variable = temperature
    boundary = right
    value = 1
  [../]
[]

[AuxVariables]
  [./vx]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./vx_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vx
    component = x
  [../]
[]

[Functions]
  [./source]
    type = ParsedFunction
    value = '10*exp(-5*x)-4*exp(-1*x)'
  [../]
[]

[Variables]
  [./pressure]
  [../]
  [./temperature]
  [../]
[]

[Kernels]
  [./H_diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
  [./T_diff]
    type = TigerThermalDiffusionKernelT
    variable = temperature
  [../]
  [./T_advect]
    type = TigerThermalAdvectionKernelT
    variable = temperature
    pressure = pressure
  [../]
  [./T_body]
    type = TigerThermalSourceKernelT
    variable = temperature
    function = source
  [../]
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
  print_linear_residuals = true
[]
