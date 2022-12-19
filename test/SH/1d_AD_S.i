[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
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
    kf_uo = rock_uo
  [../]
  [./rock_s]
    type = TigerSoluteMaterialS
    has_supg = true
    supg_uo = supg
    diffusion = 1e-6
    dispersion_longitudinal = 0
    dispersion_transverse = 0
  [../]
[]

[BCs]
  [./front_p]
    type =  DirichletBC
    variable = pressure
    boundary = left
    value = 0
  [../]
  [./back_p]
    type =  DirichletBC
    variable = pressure
    boundary = right
    value = 1000
  [../]
  [./front_s]
    type =  DirichletBC
    variable = solute
    boundary = left
    value = 0
  [../]
  [./back_s]
    type =  DirichletBC
    variable = solute
    boundary = right
    value = 100
  [../]
[]

[ICs]
  [./solute]
    type = BoundingBoxIC
    variable = solute
    inside = 0
    outside = 100
    boundary = right
    x1 = 0
    x2 = 0.999
    y1 = -1
    y2 = 1
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
  [./pe]
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
    execute_on = timestep_end
  [../]
  [./pe_ker]
    type = MaterialRealAux
    property = 'solute_peclet_number'
    variable = pe
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
  end_time = 2.5e5
  nl_abs_tol = 1e-14
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  [./TimeStepper]
    type = SolutionTimeAdaptiveDT
    dt = 20
    percent_change = 0.5
  [../]
[]

[Outputs]
  exodus = true
[]
