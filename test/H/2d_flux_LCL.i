[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 0.2
  ymin = 0
  ymax = 0.2
  nx = 3
  ny = 3
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
      viscosity = 0.0013
    [../]
  [../]
[]

[UserObjects]
  [./rock_uo]
    type =  TigerPermeabilityCubicLaw
  [../]
[]

[Materials]
  [./rock_g]
    type = TigerGeometryMaterial
    scale_factor = 3e-3
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0
    specific_density = 2500
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_h]
    type = TigerHydraulicMaterialH
    pressure = pressure
    compressibility = 7.5e-8
    kf_uo = rock_uo
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = pressure
    boundary = left
    value = 0.5
  [../]
  [./right]
    type = DirichletBC
    variable = pressure
    boundary = right
    value = 0.0
  [../]
[]

[Functions]
  [./analytical_function]
    type = ParsedFunction
    value = '(((3e-3)^2)/(12*(1.3e-3)))*(0.5/0.2)'
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

[Variables]
  [./pressure]
  [../]
[]

[Kernels]
  [./diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
[]

[Executioner]
  type = Steady
  l_tol = 1e-15 #difference between first and last linear step
  nl_rel_step_tol = 1e-7 #machine percision
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Postprocessors]
  [./error]
    type = ElementL2Error
    variable = vx
    function = analytical_function
  [../]
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]
