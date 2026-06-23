{
  uses_user_defaults: true,
  security_policy_supported_versions: { '0.0.x': ':white_check_mark:' },
  project_name: 'tnfoview',
  version: '0.1.0',
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
        'version>=': '6.8.3',
      },
    ],
  },
  snapcraft+: {
    parts+: {
      tnfoview+: {
        source: 'https://github.com/Tatsh/tnfoview.git',
        'source-tag': 'v0.1.0',
      },
    },
  },
}
