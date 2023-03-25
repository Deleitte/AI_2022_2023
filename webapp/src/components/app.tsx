import {
  AppBar,
  Box,
  Button,
  Container,
  CssBaseline,
  Toolbar,
  Typography,
  useMediaQuery,
} from "@mui/material";
import { createTheme, ThemeProvider } from "@mui/material/styles";
import { h } from "preact";
import { route, Route, Router } from "preact-router";
import Match from "preact-router/match";
import { useMemo } from "preact/hooks";

// Code-splitting is automated for `routes` directory
import Home from "../routes/home";
import Profile from "../routes/station";

const App = () => {
  const prefersDarkMode = useMediaQuery("(prefers-color-scheme: dark)");
  const theme = useMemo(
    () =>
      createTheme({
        palette: {
          mode: prefersDarkMode ? "dark" : "light",
        },
      }),
    [prefersDarkMode]
  );

  return (
    <ThemeProvider theme={theme}>
      <Box sx={{ display: "flex" }}>
        <CssBaseline />
        <AppBar position="absolute">
          <Toolbar>
            <Typography variant="h6" sx={{ flexGrow: 1 }}>
              StarGazer
            </Typography>

            <Match path="/">
              {({ matches }) =>
                !matches && <Button onClick={() => route("/")}>Go Back</Button>
              }
            </Match>
          </Toolbar>
        </AppBar>

        <Box
          component="main"
          sx={{
            backgroundColor: (theme) =>
              theme.palette.mode === "light"
                ? theme.palette.grey[100]
                : theme.palette.grey[900],
            flexGrow: 1,
            height: "100vh",
            overflow: "auto",
          }}
        >
          <Toolbar />
          <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
            <Router>
              <Route path="/" component={Home} />
              <Route path="/station/:stationId" component={Profile} />
            </Router>
          </Container>
        </Box>
      </Box>
    </ThemeProvider>
  );
};

export default App;
