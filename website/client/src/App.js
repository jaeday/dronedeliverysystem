import logo from './logo.svg';
import './App.css';
import bootstrap from 'bootstrap/dist/css/bootstrap.min.css'
import {BrowserRouter, Route, Link, Switch} from 'react-router-dom'
import Navbar from './components/Navbar'
import Homescreen from './screens/Homescreen';
import Cartscreen from './screens/Cartscreen';

function App() {
  return (
    <div className="App">
      <Navbar/>
      <BrowserRouter>
        <Route path="/" exact component ={Homescreen}/>
        <Route path="/cart" exact component ={Cartscreen}/>
      </BrowserRouter>
    </div>
  );
}

export default App;
