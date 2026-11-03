local utils = import 'utils.libsonnet';

{
  uses_user_defaults: true,
  security_policy_supported_versions: { '0.0.x': ':white_check_mark:' },
  project_name: 'tnfoview',
  version: '0.2.0',
  description: 'A simple NFO viewer.',
  custom_project_badges: [
    {
      anchor: '[![Tests](https://github.com/Tatsh/tnfoview/actions/workflows/tests.yml/badge.svg)]',
      href: 'https://github.com/Tatsh/tnfoview/actions/workflows/tests.yml',
    },
    {
      anchor: '[![Coverage Status](https://coveralls.io/repos/github/Tatsh/tnfoview/badge.svg?branch=master)]',
      href: 'https://coveralls.io/github/Tatsh/tnfoview?branch=master',
    },
  ],
  keywords: ['ascii art', 'cp437', 'nfo'],
  want_main: false,
  want_codeql: false,
  want_tests: false,
  want_winget: false,
  package_json+: {
    cspell+: {
      ignorePaths+: [
        '*.ttf',
        '.docs/*.tags',
        '.docs/*.tag.xml',
      ],
    },
    'markdownlint-cli2'+: {
      ignores: ['node_modules', '.yarn'],
    },
  },
  prettierignore+: ['*.desktop', '*.tags', '*.mm', '*.ttf'],
  gitattributes+: ['*.ttf binary'],
  cz+: {
    commitizen+: {
      version_files+: [
        'sh.tat.tnfoview.yml',
        'src/main.cpp',
        'snapcraft.yaml',
      ],
    },
  },
  vscode+: {
    c_cpp+: {
      configurations: [
        {
          cStandard: 'c23',
          compilerPath: '/usr/bin/gcc',
          cppStandard: 'c++23',
          includePath: [
            '${workspaceFolder}/src/**',
            '${workspaceFolder}/build/src/tnfoview_autogen/include',
          ],
          name: 'Linux',
        },
      ],
    },
    settings+: {
      'files.associations': {
        '*.moc': 'cpp',
        '*.ui': 'xml',
        'i18n/*.ts': 'xml',
      },
    },
  },
  // C++ only
  cmake+: {
    uses_qt: true,
  },
  project_type: 'c++',
  vcpkg+: {
    dependencies: [
      {
        features: ['gui', 'widgets'],
        name: 'qtbase',
        'version>=': utils.latestVcpkgPortVersion('qtbase'),
      },
    ],
  },
  snapcraft+: {
    parts+: {
      tnfoview+: {
        source: 'https://github.com/Tatsh/tnfoview.git',
        'source-tag': 'v0.2.0',
      },
    },
  },
}
+ {
  want_scan_build: true,
  github+: { workflows+: {
    scan_build+: {
      checkout_options: { 'fetch-depth': 0 },
      configure_command: 'cmake -S . -B build-scan -DCMAKE_BUILD_TYPE=Debug',
      documentation_steps: [
        {
          name: 'Install dependencies',
          run: |||
            sudo apt-get update
            sudo apt-get install -y doxygen graphviz
          |||,
        },
        {
          name: 'Configure',
          run: 'cmake -DCMAKE_BUILD_TYPE=Release -B "${GITHUB_WORKSPACE}"/build --install-prefix "${GITHUB_WORKSPACE}"/build/prefix -DBUILD_DOCS_ONLY=ON',
        },
        {
          name: 'Build documentation',
          run: 'cmake --build "${GITHUB_WORKSPACE}"/build --verbose --config Release --target docs',
        },
      ],
      env: {},
      setup_steps: [
        {
          name: 'Install build dependencies',
          run: |||
            sudo apt-get update
            sudo apt-get install -y qt6-base-dev qt6-base-dev-tools
          |||,
        },
        {
          name: 'Install current CMake',
          run: 'pipx install cmake',
        },
      ],
      workflow_file: 'pages.yml',
    },
  } },
}
