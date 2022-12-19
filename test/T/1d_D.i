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
    porosity = 0
    specific_density = 2600
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_t]
    type = TigerThermalMaterialT
    conductivity_type = isotropic
    advection_type = pure_diffusion
    lambda = 2
    specific_heat = 840
  [../]
[]

[BCs]
  [./front]
    type =  DirichletBC
    variable = temperature
    boundary = left
    value = 0
  [../]
  [./back]
    type =  DirichletBC
    variable = temperature
    boundary = right
    value = 100.0
  [../]
[]

[Variables]
  [./temperature]
    initial_condition = 50.0
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
