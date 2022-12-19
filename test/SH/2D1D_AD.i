[Mesh]
  [mesh]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = -0.01
    xmax = 0.01
    ymin = 0
    ymax = 0.75
    nx = 10
    ny = 15
  []
  [AddBlock]
    type = SubdomainBoundingBoxGenerator
    input = mesh
    block_id = 1
    bottom_left = '0 0 0'
    top_right = '0.01 0.75 0'
  []
  [AddSideset]
    type = SideSetsAroundSubdomainGenerator
    new_boundary = 7
    block = 0
    normal = '1 0 0'
    input = AddBlock
  []
  [lower]
    type = LowerDBlockFromSidesetGenerator
    new_block_id = 2
    sidesets = 7
    # boundaries = '7'
    input = AddSideset
  []
  [inject]
    type = BoundingBoxNodeSetGenerator
    new_boundary = inject
    bottom_left = '0 0.749 0'
    top_right = '0 0.751 0'
    input = lower
  []
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[ICs]
[]

[BCs]
  [./inject_s]
    type = DirichletBC
    variable = solute
    boundary = inject
    value = 1
  [../]
[]

[Functions]
  [./vel]
    type = ParsedVectorFunction
    value_x = '0'
    value_y = '-8.681e-6'
  [../]
  [./vel2]
    type = ParsedVectorFunction
    value_x = '0'
    value_y = '0'
  [../]
[]

[UserObjects]
  [./supg]
    type = TigerSUPG
    supg_coeficient = transient_brooks
  [../]
[]

[Materials]
  [./rock_g]
    type = TigerGeometryMaterial
    block = '0 1'
  [../]
  [./frac_g]
    type = TigerGeometryMaterial
    block = '2'
    scale_factor = 1.2e-4
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0.35
    specific_density = 2500
    block = '0 1'
  [../]
  [./frac_p]
    type = TigerPorosityMaterial
    porosity = 1
    specific_density = 2500
    block = '2'
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
    block = '0 1 2'
  [../]
  [./rock_s]
    type = TigerSoluteMaterialS
    output_Pe_Cr_numbers = false
    has_supg = false
    supg_uo = supg
    diffusion = 1e-10
    advection_type = user_velocity
    user_velocity = vel2
    dispersion_longitudinal = 0
    dispersion_transverse = 0
    block = '0 1'
  [../]
  [./frac_s]
    type = TigerSoluteMaterialS
    output_Pe_Cr_numbers = false
    has_supg = false
    supg_uo = supg
    diffusion = 0
    advection_type = user_velocity
    user_velocity = vel
    dispersion_longitudinal = 0.76
    dispersion_transverse = 0
    block = '2'
  [../]
[]

[AuxVariables]
[]

[AuxKernels]
[]

[Variables]
  [./solute]
    initial_condition = 0
  [../]
[]

[Kernels]
  [./S_advect]
    type = TigerSoluteAdvectionKernelS
    variable = solute
  [../]
  [./S_diff]
    type = TigerSoluteDiffusionKernelS
    variable = solute
  [../]
  [./S_dt]
    type = TigerSoluteTimeKernelS
    variable = solute
  [../]
[]

[Executioner]
  type = Transient
  dt = 864
  end_time = 86400
  solve_type = NEWTON
  [./TimeIntegrator]
     type = CrankNicolson
  [../]
  petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
  petsc_options_value = 'gmres asm 1E-12 1E-12 200 500 lu NONZERO'
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
  interval = 10
  file_base = 2D1D_AD
[]
