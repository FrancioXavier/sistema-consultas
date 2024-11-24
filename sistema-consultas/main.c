#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constantes
#define MAX_PACIENTES 1000
#define MAX_SALAS 100
#define MAX_ESPECIALIDADES 50
#define MAX_MEDICOS 200
#define MAX_CONSULTAS 5000

// Estruturas
typedef struct
{
    int id;
    char nome[50];
    int idade;
    float altura;
    float peso;
    char sintomas[100];
    int prioridade;
} Paciente;

typedef struct
{
    int id;
    char nome[50];
    int especialidadeId;
    int horasTrabalhadas;
} Medico;

typedef struct
{
    int id;
    char nome[50];
} Sala;

typedef struct
{
    int pacienteId;
    int medicoId;
    int salaId;
    int horario;
    int retorno;
    struct tm dataConsulta; // Data da consulta
    struct tm dataRetorno;  // Data do retorno
} Consulta;

// Função para ler dados
void lerDados(Paciente *pacientes, int *numPacientes, Medico *medicos, int *numMedicos, Sala *salas, int *numSalas)
{
    FILE *arquivo = fopen("entrada.txt", "r");
    if (!arquivo)
    {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return;
    }

    char linha[256];

    // Lendo pacientes
    fgets(linha, sizeof(linha), arquivo);
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (strncmp(linha, "Salas:", 6) == 0)
            break; // Próxima seção
        if (*numPacientes >= MAX_PACIENTES)
        {
            printf("Número máximo de pacientes excedido.\n");
            break;
        }
        sscanf(linha, "%d %s %d %f %f %s %d",
               &pacientes[*numPacientes].id,
               pacientes[*numPacientes].nome,
               &pacientes[*numPacientes].idade,
               &pacientes[*numPacientes].altura,
               &pacientes[*numPacientes].peso,
               pacientes[*numPacientes].sintomas,
               &pacientes[*numPacientes].prioridade);
        (*numPacientes)++;
    }

    // Lendo salas
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (strncmp(linha, "Especialidades:", 15) == 0)
            break; // Próxima seção
        if (*numSalas >= MAX_SALAS)
        {
            printf("Número máximo de salas excedido.\n");
            break;
        }
        sscanf(linha, "%d %s",
               &salas[*numSalas].id,
               salas[*numSalas].nome);
        (*numSalas)++;
    }

    // Lendo especialidades
    int numEspecialidades = 0;
    char especialidades[MAX_ESPECIALIDADES][50];
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (strncmp(linha, "Médicos:", 8) == 0)
            break; // Próxima seção
        if (numEspecialidades >= MAX_ESPECIALIDADES)
        {
            printf("Número máximo de especialidades excedido.\n");
            break;
        }
        sscanf(linha, "%d %s",
               &numEspecialidades,
               especialidades[numEspecialidades]);
        numEspecialidades++;
    }

    // Lendo médicos
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (*numMedicos >= MAX_MEDICOS)
        {
            printf("Número máximo de médicos excedido.\n");
            break;
        }
        sscanf(linha, "%d %s %d", &medicos[(*numMedicos)].id, medicos[(*numMedicos)].nome, &medicos[(*numMedicos)].especialidadeId);

        medicos[*numMedicos].horasTrabalhadas = 0; // Inicializa horas trabalhadas
        (*numMedicos)++;
    }

    fclose(arquivo);
    printf("Dados lidos do arquivo com sucesso!\n");
    printf("Pacientes lidos: %d\n", *numPacientes);
    printf("Salas lidas: %d\n", *numSalas);
    printf("Especialidades lidas: %d\n", numEspecialidades);
    printf("Médicos lidos: %d\n", *numMedicos);
}

// Função para alocar consultas
void alocarConsultas(Paciente *pacientes, int numPacientes, Medico *medicos, int numMedicos,
                     Sala *salas, int numSalas, Consulta *consultas, int *numConsultas)
{
    // Matrizes para rastrear disponibilidade de médicos e salas.
    int horariosMedicos[MAX_MEDICOS][17] = {0}; // Horários ocupados pelos médicos (8h às 16h).
    int horariosSalas[MAX_SALAS][17] = {0};     // Horários ocupados pelas salas (8h às 16h).

    // Inicializa as horas trabalhadas dos médicos
    for (int m = 0; m < numMedicos; m++)
    {
        medicos[m].horasTrabalhadas = 0;
    }

    for (int i = 0; i < numPacientes; i++)
    {
        if (*numConsultas >= MAX_CONSULTAS)
            break;

        int consultaAlocada = 0; // Flag para saber se a consulta foi alocada.

        // Tenta alocar consulta para o paciente
        for (int h = 8; h < 17 && !consultaAlocada; h++)
        { // Horários de 8h às 16h.
            for (int m = 0; m < numMedicos && !consultaAlocada; m++)
            {
                for (int s = 0; s < numSalas && !consultaAlocada; s++)
                {
                    // Verifica se médico e sala estão disponíveis no horário.
                    if (horariosMedicos[m][h] == 0 && horariosSalas[s][h] == 0)
                    {
                        // Cria uma nova consulta.
                        Consulta novaConsulta = {
                            .pacienteId = pacientes[i].id,
                            .medicoId = medicos[m].id,
                            .salaId = salas[s].id,
                            .horario = h,
                            .retorno = 30 // Retorno inicial 30 dias após a consulta
                        };

                        // Atualiza as matrizes de disponibilidade.
                        horariosMedicos[m][h] = 1;
                        horariosSalas[s][h] = 1;

                        // Adiciona a consulta à lista.
                        consultas[(*numConsultas)++] = novaConsulta;

                        // Incrementa as horas trabalhadas do médico.
                        medicos[m].horasTrabalhadas++;

                        consultaAlocada = 1; // Consulta foi alocada.
                    }
                }
            }
        }

        // Caso não seja possível alocar a consulta.
        if (!consultaAlocada)
        {
            printf("Erro: Não foi possível alocar consulta para o paciente %d.\n", pacientes[i].id);
        }
    }
}

// Função auxiliar para determinar o dia da semana
const char *obterDiaDaSemana(int horario)
{
    // Cada dia tem 9 horas (8h às 17h), assumindo que o expediente começa em "horário = 8"
    int diaIndex = (horario - 8) / 9;
    const char *diasDaSemana[] = {"Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sábado"};
    return diasDaSemana[diaIndex % 7]; // Cicla entre os dias úteis
}

// Função ajustada para gerar relatório
void gerarRelatorio(Consulta *consultas, int numConsultas, Medico *medicos, int numMedicos, Paciente *pacientes, int numPacientes)
{
    FILE *arquivo = fopen("relatorio.txt", "w");
    if (!arquivo)
    {
        printf("Erro ao abrir arquivo para salvar o relatório.\n");
        return;
    }

    fprintf(arquivo, "=== Relatório de Consultas ===\n\n");
    fprintf(arquivo, "Consultas Realizadas por Dia da Semana:\n");

    const char *diasDaSemana[] = {"Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sábado"};

    for (int d = 0; d < 7; d++)
    { // Itera pelos dias úteis
        fprintf(arquivo, "\n%s:\n", diasDaSemana[d]);
        int consultasNoDia = 0;

        char bufferConsulta[20], bufferRetorno[20];
        for (int i = (d); i < numConsultas; i++)
        {
            // Obter a data atual
            time_t t = time(NULL);
            struct tm dataAtual = *localtime(&t);

            // Definir a data da consulta como a data atual
            consultas[i].dataConsulta = dataAtual;

            // Calcular a data de retorno (30 dias após a data da consulta)
            struct tm dataRetorno = dataAtual;
            dataRetorno.tm_mday += 30;

            // Ajustar a data de retorno caso ultrapasse o limite do mês
            mktime(&dataRetorno); // Ajusta automaticamente o mês/ano se necessário

            consultas[i].dataRetorno = dataRetorno; // Indica que o retorno foi agendado
            // Formatar datas da consulta e do retorno
            strftime(bufferConsulta, sizeof(bufferConsulta), "%d/%m/%Y", &consultas[i].dataConsulta);
            strftime(bufferRetorno, sizeof(bufferRetorno), "%d/%m/%Y", &consultas[i].dataRetorno);
            // Ignora consultas inválidas
            if (consultas[i].pacienteId == 0 || consultas[i].medicoId == 0 || consultas[i].salaId == 0 || consultas[i].horario == 0)
            {
                continue;
            }

            const char *diaConsulta = obterDiaDaSemana(consultas[i].horario);
            if (strcmp(diaConsulta, diasDaSemana[d]) == 0)
            {
                // Recupera o nome do paciente
                char nomePaciente[50] = "Desconhecido";
                for (int p = 0; p < numPacientes; p++)
                {
                    if (pacientes[p].id == consultas[i].pacienteId)
                    {
                        strcpy(nomePaciente, pacientes[p].nome);
                        break;
                    }
                }

                // Recupera o nome do médico
                char nomeMedico[50] = "Desconhecido";
                for (int m = 0; m < numMedicos; m++)
                {
                    if (medicos[m].id == consultas[i].medicoId)
                    {
                        strcpy(nomeMedico, medicos[m].nome);
                        break;
                    }
                }

                int dias = consultas[i].horario % 24;
                if (consultas[i].retorno != 0)
                {
                    fprintf(arquivo, "Consulta %d: Paciente %d, Médico %d, Sala %d, Horário %d, Retorno em %d dias\n",
                            i + 1,
                            consultas[i].pacienteId,
                            consultas[i].medicoId,
                            consultas[i].salaId,
                            dias,
                            consultas[i].retorno);
                    consultasNoDia++;
                }
            }
        }

        if (consultasNoDia == 0)
        {
            fprintf(arquivo, "  Nenhuma consulta agendada.\n");
        }
    }

    fprintf(arquivo, "\n\n=== Consultas de retorno ===\n\n");
    char bufferConsulta[20], bufferRetorno[20];
    for (int i = 0; i < numConsultas; i++)
    {
        // Formatar datas da consulta e do retorno
        strftime(bufferConsulta, sizeof(bufferConsulta), "%d/%m/%Y", &consultas[i].dataConsulta);
        strftime(bufferRetorno, sizeof(bufferRetorno), "%d/%m/%Y", &consultas[i].dataRetorno);

        int horario = consultas[i].horario % 24;
        int dias = consultas[i].horario / 24;
        if (consultas[i].retorno == 0)
        {
            fprintf(arquivo, "Consulta %d: Paciente: %d, Médico: %d, Sala: %d, dia e hora: %d:00, %s \n",
                    i + 1,
                    consultas[i].pacienteId,
                    consultas[i].medicoId,
                    consultas[i].salaId,
                    horario,
                    bufferRetorno);
        }
    }

    fprintf(arquivo, "\nResumo de Horas Trabalhadas:\n");
    for (int i = 0; i < numMedicos; i++)
    {
        fprintf(arquivo, "- Médico %s (ID %d): %d %s\n",
                medicos[i].nome, medicos[i].id, medicos[i].horasTrabalhadas,
                medicos[i].horasTrabalhadas == 1 ? "hora" : "horas");
    }

    fclose(arquivo);
    printf("Relatório salvo em 'relatorio.txt'.\n");
}

void gerenciarRetornos(Consulta *consultas, int *numConsultas, int maxConsultas)
{
    printf("Gerenciando retornos...\n");

    // Iterar sobre todas as consultas existentes
    for (int i = 0; i < *numConsultas; i++)
    {
        // Verificar se a consulta original tem um retorno agendado
        if (consultas[i].retorno != 0)
        {
            // Calcular data de retorno: adicionar 30 dias à data da consulta
            consultas[i].dataRetorno = consultas[i].dataConsulta; // Copiar data original
            consultas[i].dataRetorno.tm_mday += 30;

            // Ajustar para uma data válida
            mktime(&consultas[i].dataRetorno); // Normaliza a data (ajusta mês e ano automaticamente)
            // Criar uma nova consulta de retorno
            if (*numConsultas < maxConsultas)
            {
                Consulta novaConsulta = {
                    .pacienteId = consultas[i].pacienteId,
                    .medicoId = consultas[i].medicoId,
                    .salaId = consultas[i].salaId,
                    .horario = consultas[i].horario + (consultas[i].retorno * 24), // 30 dias em horas
                    .retorno = 0                                                   // Retorno do retorno não será definido
                };

                // Adicionar a nova consulta à lista
                consultas[(*numConsultas)++] = novaConsulta;

                int horario = novaConsulta.horario % 24;
                int dias = novaConsulta.horario / 24;
                printf("Retorno agendado: Paciente %d com Médico %d na Sala %d as %d horas em %d dias.\n",
                       novaConsulta.pacienteId, novaConsulta.medicoId,
                       novaConsulta.salaId, horario, dias);
            }
            else
            {
                printf("Capacidade máxima de consultas atingida. Não foi possível agendar retorno.\n");
            }
        }
    }
}

// Função principal
int main()
{
    Paciente pacientes[MAX_PACIENTES];
    Medico medicos[MAX_MEDICOS];
    Sala salas[MAX_SALAS];
    Consulta consultas[MAX_CONSULTAS];
    int numPacientes = 0, numMedicos = 0, numSalas = 0, numConsultas = 0;

    // Ler dados do arquivo
    lerDados(pacientes, &numPacientes, medicos, &numMedicos, salas, &numSalas);

    // Alocar consultas
    alocarConsultas(pacientes, numPacientes, medicos, numMedicos, salas, numSalas, consultas, &numConsultas);

    // Gerenciar retornos
    gerenciarRetornos(consultas, &numConsultas, MAX_CONSULTAS);

    // Gerar relatório
    gerarRelatorio(consultas, numConsultas, medicos, numMedicos, pacientes, numPacientes);

    return 0;
}
