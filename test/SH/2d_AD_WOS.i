[Mesh]
  uniform_refine = 1
  allow_renumbering = false
  [mesh]
    type = FileMeshGenerator
    file = 2d_ad.msh
  []
  [c_side]
    type = BoundingBoxNodeSetGenerator
    input = mesh
    bottom_left = '0.49 0.49 0'
    top_right = '0.51 0.51 0'
    new_boundary = 'c'
  []
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
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
    effective_length = average
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
    specific_density = 2500
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
  [./matrix_s]
    type = TigerSoluteMaterialS
    has_supg = true
    supg_uo = supg
    diffusion = 0
  [../]
[]

[BCs]
  [./lt_h]
    type = NeumannBC
    variable = pressure
    boundary = 'l1 l2 t'
    value = 0.707106781
  [../]
  [./c_h]
    type = DirichletBC
    variable = pressure
    boundary = 'c'
    value = 0
  [../]
  [./rb_h]
    type = NeumannBC
    variable = pressure
    boundary = 'r1 r2 b'
    value = -0.707106781
  [../]
  [./left_t]
    type = DirichletBC
    variable = solute
    boundary = 'l1 t'
    value = 1
  [../]
  [./top_t]
    type = DirichletBC
    variable = solute
    boundary = 'l2'
    value = 0
  [../]
[]

[AuxVariables]
  [./vx]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./vy]
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
  [./vy_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vy
    component = y
  [../]
[]

[Variables]
  [./pressure]
  [../]
  [./solute]
  [../]
[]

[Kernels]
  [./H_diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
  [./S_diff]
    type = TigerSoluteDiffusionKernelS
    variable = solute
  [../]
  [./S_advect]
    type = TigerSoluteAdvectionKernelS
    variable = solute
    pressure = pressure
  [../]
[]

[Executioner]
  type = Steady
  nl_rel_tol = 1e-09
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  automatic_scaling = true
[]

[Outputs]
  exodus = true
  print_linear_residuals = true
[]
