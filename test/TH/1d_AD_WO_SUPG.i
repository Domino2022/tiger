[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 40
  xmax = 10
  xmin = 0
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
      density = 2600
      viscosity = 0.001
      cp = 840
    [../]
  [../]
[]

[UserObjects]
  [./rock_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-10'
  [../]
[]

[Materials]
  [./rock_g]
    type = TigerGeometryMaterial
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0
    specific_density = 2600
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_h]
    type = TigerHydraulicMaterialH
    pressure = pressure
    compressibility = 1.0e-9
    kf_uo = rock_uo
  [../]
  [./rock_t]
    type = TigerThermalMaterialT
    conductivity_type = isotropic
    lambda = 2.184
    specific_heat = 840
  [../]
[]

[BCs]
  [./left_p]
    type =  DirichletBC
    variable = pressure
    boundary = left
    value = 0
  [../]
  [./right_p]
    type =  DirichletBC
    variable = pressure
    boundary = right
    value = 100.0
  [../]
  [./left_t]
    type =  DirichletBC
    variable = temperature
    boundary = left
    value = 0
  [../]
  [./right_t]
    type =  DirichletBC
    variable = temperature
    boundary = right
    value = 100.0
  [../]
[]

[Variables]
  [./temperature]
  [../]
  [./pressure]
  [../]
[]

[AuxVariables]
  [./vx]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./analytical_solution]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Functions]
  [./analytical_function]
    type = ParsedFunction
    vars = 'l T'
    vals = '10 100'
    value = 'T*exp((l-x)/2)*sinh(x/2)/sinh(l/2)'
  [../]
[]

[AuxKernels]
  [./vx_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vx
    component = x
  [../]
  [./a_ker]
    type = FunctionAux
    function = analytical_function
    variable = analytical_solution
    execute_on = initial
  [../]
[]

[Kernels]
  [./T_diff]
    type = TigerThermalDiffusionKernelT
    variable = temperature
  [../]
  [./T_advect]
    type = TigerThermalAdvectionKernelT
    variable = temperature
    pressure = pressure
  [../]
  [./H_diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
[]

[Executioner]
  type = Steady
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
