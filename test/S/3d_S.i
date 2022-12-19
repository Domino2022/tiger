[Mesh]
  type = FileMesh
  file = 3d_curvedfrac.msh
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[GlobalParams]
[]

[Materials]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_g1]
    type = TigerGeometryMaterial
    block = 'b1 b2'
  [../]
  [./rock_p1]
    type = TigerPorosityMaterial
    porosity = 1
    specific_density = 2500
    block = 'b1 b2'
  [../]
  [./rock_g2]
    type = TigerGeometryMaterial
    scale_factor = 0.1
    block = frac
  [../]
  [./rock_p2]
    type = TigerPorosityMaterial
    porosity = 1
    specific_density = 2500
    block = frac
  [../]
  [./rock_s]
    type = TigerSoluteMaterialS
    advection_type = pure_diffusion
    diffusion = 1e-6
    output_properties = 'dispersion_tensor diffusion_factor diffusion_dispersion TimeKernel_S'
  [../]
[]

[BCs]
  [./front]
    type =  DirichletBC
    variable = solute
    boundary = top
    value = 0
  [../]
  [./back]
    type =  DirichletBC
    variable = solute
    boundary = bottom
    value = 100.0
  [../]
[]

[Variables]
  [./solute]
    initial_condition = 0.0
  [../]
[]

[Kernels]
  [./T_diff]
    type = TigerSoluteDiffusionKernelS
    variable = solute
  [../]
  [./T_dt]
    type = TigerSoluteTimeKernelS
    variable = solute
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10
  end_time = 1.0e5
  l_tol = 1e-10 #difference between first and last linear step
  nl_rel_step_tol = 1e-14 #machine percision
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
