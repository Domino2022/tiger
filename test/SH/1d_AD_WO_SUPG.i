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
    porosity = 1
    specific_density = 2500
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
  [./rock_s]
    type = TigerSoluteMaterialS
    diffusion = 1e-3
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
    variable = solute
    boundary = left
    value = 0
  [../]
  [./right_t]
    type =  DirichletBC
    variable = solute
    boundary = right
    value = 100.0
  [../]
[]

[Variables]
  [./solute]
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
[]

[AuxKernels]
  [./vx_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vx
    component = x
  [../]
[]

[Kernels]
  [./S_diff]
    type = TigerSoluteDiffusionKernelS
    variable = solute
  [../]
  [./S_advect]
    type = TigerSoluteAdvectionKernelS
    variable = solute
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
