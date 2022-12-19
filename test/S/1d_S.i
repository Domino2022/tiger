[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
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
    porosity = 1
    specific_density = 2500
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_s]
    type = TigerSoluteMaterialS
    diffusion = 1e-6
    dispersion_longitudinal = 0
    dispersion_transverse = 0
    advection_type = pure_diffusion
    formation_factor = 1
    output_properties = 'dispersion_tensor diffusion_factor diffusion_dispersion TimeKernel_S'
  [../]
[]

[BCs]
  [./front]
    type =  DirichletBC
    variable = solute
    boundary = left
    value = 0
  [../]
  [./back]
    type =  DirichletBC
    variable = solute
    boundary = right
    value = 100.0
  [../]
[]

[Variables]
  [./solute]
    initial_condition = 0
  [../]
[]

[Kernels]
  [./S_dt]
    type = TigerSoluteTimeKernelS
    variable = solute
  [../]
  [./S_diff]
    type = TigerSoluteDiffusionKernelS
    variable = solute
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 50
  end_time = 2.0e5
  l_tol = 1e-10 #difference between first and last linear step
  nl_rel_step_tol = 1e-14 #machine percision
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
