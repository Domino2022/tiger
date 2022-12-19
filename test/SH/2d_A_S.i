[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  nx = 20
  ny = 20
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[ICs]
  [./pressure]
    type = FunctionIC
    variable = pressure
    function = p_init
  [../]
[]

[Functions]
  [./p_init]
    type = ParsedFunction
    value = 'y*1e6'
  [../]
[]

[UserObjects]
  [./perm_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-10'
  [../]
  [./supg]
    type = TigerSUPG
    effective_length = min
    supg_coeficient = transient_tezduyar
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
    compressibility = 1.0e-4
    kf_uo = perm_uo
  [../]
  [./rock_s]
    type = TigerSoluteMaterialS
    has_supg = true
    supg_uo = supg
    diffusion = 1e-5
    dispersion_longitudinal = 0
    dispersion_transverse = 0
  [../]
[]

[BCs]
  [./top_s]
    type = DirichletBC
    variable = solute
    boundary = 'top'
    value = 1
  [../]
  [./bottom_s]
    type = NeumannBC
    variable = solute
    boundary = 'bottom'
    value = 0
  [../]
  [./top_p]
    type = DirichletBC
    variable = pressure
    boundary = 'top'
    value = 1e6
  [../]
  [./bottom_p]
    type = DirichletBC
    variable = pressure
    boundary = 'bottom'
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
  [./pe]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./cr]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./vx_ker]
    type = MaterialRealVectorValueAux
    property = 'solute_advection_velocity'
    variable = vx
    component = 0
  [../]
  [./vy_ker]
    type = MaterialRealVectorValueAux
    property = 'solute_advection_velocity'
    variable = vy
    component = 1
  [../]
[]

[Variables]
  [./solute]
    initial_condition = 0
  [../]
  [./pressure]
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
  [./S_dt]
    type = TigerSoluteTimeKernelS
    variable = solute
  [../]
  [./H_diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
  [./H_dt]
    type = TigerHydraulicTimeKernelH
    variable = pressure
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.5
  end_time = 10
  solve_type = NEWTON
  petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
  petsc_options_value = 'gmres asm 1E-10 1E-10 200 500 lu NONZERO'
[]

[Outputs]
  exodus = true
  interval = 2
[]
