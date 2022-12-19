[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 100
  zmax = 0
  zmin = -1000
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[Materials]
  [./rock_g]
    type = TigerGeometryMaterial
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0
    specific_density = 0.1 #artificial value to reach steady in the transient simulation
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_t]
    type = TigerThermalMaterialT
    advection_type = pure_diffusion
    conductivity_type = isotropic
    lambda = 3
    specific_heat = 0.1 #artificial value to reach steady in the transient simulation
  [../]
[]

[BCs]
  [./front]
    type =  DirichletBC
    variable = temperature
    boundary = 'front'
    value = 10.0
  [../]
  [./back_n]
    type =  NeumannBC
    variable = temperature
    boundary = 'back'
    value = 8.0e-2
  [../]
[]

[AuxVariables]
  [./analytical_solution]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Functions]
  [./analytical_function]
    type = ParsedFunction
    vars = 'q T D A l'
    vals = '8.0e-2 10 3 1e-6 1000'
    value = 'T - (q+A*l)/D*z - A*z*z/2/D'
  [../]
[]

[AuxKernels]
  [./a_ker]
    type = FunctionAux
    function = analytical_function
    variable = analytical_solution
    execute_on = initial
  [../]
[]

[Variables]
  [./temperature]
    initial_condition = 10.0
  [../]
[]

[Kernels]
  [./T_diff]
    type = TigerThermalDiffusionKernelT
    variable = temperature
  [../]
  [./T_dt]
    type = TigerThermalTimeKernelT
    variable = temperature
  [../]
  [./T_Source]
    type = TigerThermalSourceKernelT
    variable = temperature
    value = 1.0e-6
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10
  end_time = 2.5e4
  #type = Steady
  l_tol = 1e-10 #difference between first and last linear step
  nl_rel_step_tol = 1e-10 #machine percision
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
   print_linear_residuals = false
[]
